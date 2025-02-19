#ifdef _WINDOWS
#include <windows.h>
#endif

#include "Quantize.h"

CQuantizer::CQuantizer(UINT nMaxColors, UINT nColorBits)
{
	m_nColorBits = nColorBits < 8 ? nColorBits : 8;

	m_pTree = nullptr;
	m_nLeafCount = 0;
	for (int i = 0; i <= static_cast<int>(m_nColorBits); i++)
		m_pReducibleNodes[i] = nullptr;
	m_nMaxColors = m_nOutputMaxColors = nMaxColors;
	if (m_nMaxColors < 16) m_nMaxColors = 16;
}

CQuantizer::~CQuantizer()
{
	if (m_pTree != nullptr)
		DeleteTree(&m_pTree);
}

BOOL CQuantizer::ProcessImage(HANDLE hImage)
{
	BYTE r, g, b;
	int i, j;

	BITMAPINFOHEADER ds;
	memcpy(&ds, (void*)hImage, sizeof(ds));
	const int effwdt = ((((ds.biBitCount * ds.biWidth) + 31) / 32) * 4);

	const int nPad = effwdt - (((ds.biWidth * ds.biBitCount) + 7) / 8);

	BYTE* pbBits = static_cast<BYTE*>(hImage) + *static_cast<DWORD*>(hImage) + ds.biClrUsed * sizeof(RGBQUAD);

	switch (ds.biBitCount)
	{
	case 1: // 1-bit DIB
	case 4: // 4-bit DIB
	case 8: // 8-bit DIB
		for (i = 0; i < ds.biHeight; i++)
		{
			for (j = 0; j < ds.biWidth; j++)
			{
				const BYTE idx = GetPixelIndex(j, i, ds.biBitCount, effwdt, pbBits);
				const BYTE* pal = static_cast<BYTE*>(hImage) + sizeof(BITMAPINFOHEADER);
				const long ldx = idx * sizeof(RGBQUAD);
				b = pal[ldx];
				g = pal[ldx + 1];
				r = pal[ldx + 2];
				const BYTE a = pal[ldx + 3];
				AddColor(&m_pTree, r, g, b, a, m_nColorBits, 0, &m_nLeafCount,
				         m_pReducibleNodes);
				while (m_nLeafCount > m_nMaxColors)
					ReduceTree(m_nColorBits, &m_nLeafCount,
					           m_pReducibleNodes);
			}
		}

		break;
	case 24: //  24-bit DIB
		for (i = 0; i < ds.biHeight; i++)
		{
			for (j = 0; j < ds.biWidth; j++)
			{
				b = *pbBits++;
				g = *pbBits++;
				r = *pbBits++;
				AddColor(&m_pTree, r, g, b, 0, m_nColorBits, 0, &m_nLeafCount,
				         m_pReducibleNodes);
				while (m_nLeafCount > m_nMaxColors)
					ReduceTree(m_nColorBits, &m_nLeafCount, m_pReducibleNodes);
			}
			pbBits += nPad;
		}
		break;

	default: //  Unrecognized color format
		return FALSE;
	}
	return TRUE;
}

void CQuantizer::AddColor(NODE** ppNode, BYTE r, BYTE g, BYTE b, BYTE a,
                          UINT nColorBits, UINT nLevel, UINT* pLeafCount, NODE** pReducibleNodes)
{
	static BYTE mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	// If the node doesn't exist, create it.
	if (*ppNode == nullptr)
		*ppNode = static_cast<NODE*>(CreateNode(nLevel, nColorBits, pLeafCount, pReducibleNodes));

	// Update color  information  if it's  a leaf node.
	if ((*ppNode)->bIsLeaf)
	{
		(*ppNode)->nPixelCount++;
		(*ppNode)->nRedSum += r;
		(*ppNode)->nGreenSum += g;
		(*ppNode)->nBlueSum += b;
		(*ppNode)->nAlphaSum += a;
	}
	else
	{
		// Recurse a level deeper if the node is not a leaf.
		const int shift = 7 - nLevel;
		const int nIndex = (((r & mask[nLevel]) >> shift) << 2) |
			(((g & mask[nLevel]) >> shift) << 1) |
			((b & mask[nLevel]) >> shift);
		AddColor(&((*ppNode)->pChild[nIndex]), r, g, b, a, nColorBits,
		         nLevel + 1, pLeafCount, pReducibleNodes);
	}
}

void* CQuantizer::CreateNode(UINT nLevel, UINT nColorBits, UINT* pLeafCount,
                             NODE** pReducibleNodes)
{
	NODE* pNode = static_cast<NODE*>(calloc(1, sizeof(NODE)));

	if (pNode == nullptr) return nullptr;

	pNode->bIsLeaf = (nLevel == nColorBits) ? TRUE : FALSE;
	if (pNode->bIsLeaf) (*pLeafCount)++;
	else
	{
		pNode->pNext = pReducibleNodes[nLevel];
		pReducibleNodes[nLevel] = pNode;
	}
	return pNode;
}

void CQuantizer::ReduceTree(UINT nColorBits, UINT* pLeafCount, NODE** pReducibleNodes)
{
	int i;
	// Find  the  deepest  level containing at  least one reducible  node.
	for (i = nColorBits - 1; (i > 0) && (pReducibleNodes[i] == nullptr); i--);

	// Reduce the node most  recently added to the list at level  i.
	NODE* pNode = pReducibleNodes[i];
	pReducibleNodes[i] = pNode->pNext;

	UINT nRedSum = 0;
	UINT nGreenSum = 0;
	UINT nBlueSum = 0;
	UINT nAlphaSum = 0;
	UINT nChildren = 0;

	for (i = 0; i < 8; i++)
	{
		if (pNode->pChild[i] != nullptr)
		{
			nRedSum += pNode->pChild[i]->nRedSum;
			nGreenSum += pNode->pChild[i]->nGreenSum;
			nBlueSum += pNode->pChild[i]->nBlueSum;
			nAlphaSum += pNode->pChild[i]->nAlphaSum;
			pNode->nPixelCount += pNode->pChild[i]->nPixelCount;
			free(pNode->pChild[i]);
			pNode->pChild[i] = nullptr;
			nChildren++;
		}
	}

	pNode->bIsLeaf = TRUE;
	pNode->nRedSum = nRedSum;
	pNode->nGreenSum = nGreenSum;
	pNode->nBlueSum = nBlueSum;
	pNode->nAlphaSum = nAlphaSum;
	*pLeafCount -= (nChildren - 1);
}

void CQuantizer::DeleteTree(NODE** ppNode)
{
	for (auto& i : (*ppNode)->pChild)
	{
		if (i != nullptr) DeleteTree(&i);
	}
	free(*ppNode);
	*ppNode = nullptr;
}

void CQuantizer::GetPaletteColors(NODE* pTree, RGBQUAD* prgb, UINT* pIndex, UINT* pSum)
{
	if (pTree)
	{
		if (pTree->bIsLeaf)
		{
			prgb[*pIndex].rgbRed = static_cast<BYTE>((pTree->nRedSum) / (pTree->nPixelCount));
			prgb[*pIndex].rgbGreen = static_cast<BYTE>((pTree->nGreenSum) / (pTree->nPixelCount));
			prgb[*pIndex].rgbBlue = static_cast<BYTE>((pTree->nBlueSum) / (pTree->nPixelCount));
			prgb[*pIndex].rgbReserved = static_cast<BYTE>((pTree->nAlphaSum) / (pTree->nPixelCount));
			if (pSum) pSum[*pIndex] = pTree->nPixelCount;
			(*pIndex)++;
		}
		else
		{
			for (const auto& i : pTree->pChild)
			{
				if (i != nullptr)
					GetPaletteColors(i, prgb, pIndex, pSum);
			}
		}
	}
}

UINT CQuantizer::GetColorCount()
{
	return m_nLeafCount;
}

void CQuantizer::SetColorTable(RGBQUAD* prgb)
{
	UINT nIndex = 0;
	if (m_nOutputMaxColors < 16)
	{
		UINT nSum[16];
		RGBQUAD tmppal[16];
		GetPaletteColors(m_pTree, tmppal, &nIndex, nSum);
		if (m_nLeafCount > m_nOutputMaxColors)
		{
			UINT ng, nb, na, ns;
			for (UINT j = 0; j < m_nOutputMaxColors; j++)
			{
				const UINT a = (j * m_nLeafCount) / m_nOutputMaxColors;
				const UINT b = ((j + 1) * m_nLeafCount) / m_nOutputMaxColors;
				UINT nr = ng = nb = na = ns = 0;
				for (UINT k = a; k < b; k++)
				{
					nr += tmppal[k].rgbRed * nSum[k];
					ng += tmppal[k].rgbGreen * nSum[k];
					nb += tmppal[k].rgbBlue * nSum[k];
					na += tmppal[k].rgbReserved * nSum[k];
					ns += nSum[k];
				}
				prgb[j].rgbRed = nr / ns;
				prgb[j].rgbGreen = ng / ns;
				prgb[j].rgbBlue = nb / ns;
				prgb[j].rgbReserved = na / ns;
			}
		}
		else
		{
			memcpy(prgb, tmppal, m_nLeafCount * sizeof(RGBQUAD));
		}
	}
	else
	{
		GetPaletteColors(m_pTree, prgb, &nIndex, nullptr);
	}
}

BYTE CQuantizer::GetPixelIndex(long x, long y, int nbit, long effwdt, BYTE* pimage)
{
	if (nbit == 8)
	{
		return pimage[y * effwdt + x];
	}
	else
	{
		BYTE pos;
		BYTE iDst = pimage[y * effwdt + (x * nbit >> 3)];
		if (nbit == 4)
		{
			pos = static_cast<BYTE>(4 * (1 - x % 2));
			iDst &= (0x0F << pos);
			return (iDst >> pos);
		}
		else if (nbit == 1)
		{
			pos = static_cast<BYTE>(7 - x % 8);
			iDst &= (0x01 << pos);
			return (iDst >> pos);
		}
	}
	return 0;
}
