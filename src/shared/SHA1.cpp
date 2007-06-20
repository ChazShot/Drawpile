/******************************************************************************

   Modified by M.K.A. <wyrmchild@users.sourceforge.net> - 2006-12-12
   Removed rest of the macros in 2007-01-12
   
   ---
   
   100% free public domain implementation of the SHA-1 algorithm
   by Dominik Reichl <dominik.reichl@t-online.de>
   Web: http://www.dominik-reichl.de/
   
   Version 1.6 - 2005-02-07

******************************************************************************/

#include "templates.h"
#include "SHA1.h"

#include <cassert>
#include <memory.h> // memset(), memcpy()

SHA1::SHA1() throw()
{
	Reset();
}

void SHA1::Reset() throw()
{
	// SHA1 initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xEFCDAB89;
	m_state[2] = 0x98BADCFE;
	m_state[3] = 0x10325476;
	m_state[4] = 0xC3D2E1F0;
	
	m_count = 0;
	m_size = 0;
	
	#ifndef NDEBUG
	finalized = false;
	#endif
}

// Rotate \b v n bits to the left
uint32_t SHA1::ROL32(const uint32_t v, const uint32_t n) const throw()
{
	#ifdef USE_ASM
	uint32_t x;
	asm (
		"rol %%cl, %0;"
		: "=r"(x)
		: "0"(v), "c"(n)
	);
	return x;
	#else
	return (v << n) | (v >> (32 - n));
	#endif
}

uint32_t SHA1::SHABLK1(const uint32_t i) throw()
{
	return (m_block.l[i&15] = ROL32((m_block.l[(i+13)&15] ^ m_block.l[(i+8)&15] ^ m_block.l[(i+2)&15] ^ m_block.l[i&15]), 1));
}

void SHA1::R0(const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const uint32_t i) throw()
{
	z += ((w & (x ^ y)) ^ y) + m_block.l[i] + 0x5A827999 + ROL32(v, 5);
	w = ROL32(w, 30);
}

void SHA1::R1(const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const uint32_t i) throw()
{
	z += ((w & (x ^ y)) ^ y) + SHABLK1(i) + 0x5A827999 + ROL32(v, 5);
	w = ROL32(w, 30);
}

void SHA1::R2(const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const uint32_t i) throw()
{
	z += (w ^ x ^ y) + SHABLK1(i) + 0x6ED9EBA1 + ROL32(v, 5);
	w = ROL32(w, 30);
}

void SHA1::R3(const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const uint32_t i) throw()
{
	z += (((w | x) & y) | (w & x)) + SHABLK1(i) + 0x8F1BBCDC + ROL32(v, 5);
	w = ROL32(w, 30);
}

void SHA1::R4(const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const uint32_t i) throw()
{
	z += (w ^ x ^ y) + SHABLK1(i) + 0xCA62C1D6 + ROL32(v, 5);
	w = ROL32(w, 30);
}

void SHA1::Transform(const uchar *buffer) throw()
{
	assert(buffer != 0);
	
	// Copy state[] to working vars
	uint32_t a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3], e = m_state[4];
	
	memcpy(m_block.c, buffer, sizeof(m_block.c));
	
	#ifndef IS_BIG_ENDIAN
	for (int i=0; i != 16; ++i)
		bswap(m_block.l[i]);
	#endif
	
	// 4 rounds of 20 operations each. Loop unrolled.
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	
	// Add the working vars back into state
	m_state[0] += a;
	m_state[1] += b;
	m_state[2] += c;
	m_state[3] += d;
	m_state[4] += e;
}

// Use this function to hash in binary data and strings
void SHA1::Update(const uchar *data, const uint32_t len) throw()
{
	assert(len >= 0);
	assert(data != 0);
	
	assert(not finalized);
	
	static uchar m_buffer[64];
	
	const uint32_t left = m_size & 63;
	const uint64_t available = left + len;
	m_size += len;
	
	m_count += (len << 3);
	
	if (available < 64ULL)
		memcpy(&m_buffer[left], &data[0], len);
	else
	{
		int64_t i = 64 - left;
		memcpy(&m_buffer[left], data, i);
		Transform(m_buffer);
		
		int64_t last = len - (available & 63LL);
		for (; i != last; i += 64)
			Transform(&data[i]);
		
		memcpy(&m_buffer[0], &data[i], len - i);
	}
}

void SHA1::Final() throw()
{
	assert(not finalized);
	
	union {
		uint64_t ll;
		uint8_t c[8];
	} swb = {m_size << 3};
	
	uchar finalcount[8]
	#ifndef IS_BIG_ENDIAN
		= {swb.c[7],swb.c[6],swb.c[5],swb.c[4],swb.c[3],swb.c[2],swb.c[1],swb.c[0]};
	#else
		;
	memcpy(finalcount, &swb.ll, sizeof(m_size));
	#endif
	
	Update((const uchar *)"\200", 1);
	
	/*
	unsigned char zero[64] = {0};
	
	if (int(m_size & 63) > 56 - 1)
	{
		Update(zero, 64 - 1 - int(m_size & 63));
		Update(zero, 64 - 8);
	}
	else
		Update(zero, 64 - 1 - 8 - int(m_size & 63));
	*/
	
	while ((m_count & 504) != 448)
		Update((const uchar *)"\0", 1);
	
	Update(finalcount, 8); // Cause a SHA1Transform()
	
	bswap(m_state[0]);
	bswap(m_state[1]);
	bswap(m_state[2]);
	bswap(m_state[3]);
	bswap(m_state[4]);
	
	#ifndef NDEBUG
	finalized = true;
	#endif
}

// Get the final hash as a pre-formatted (ASCII) string (40 bytes long)
void SHA1::HexDigest(char *szReport) const throw()
{
	assert(finalized);
	assert(szReport != 0);
	
	// Hex magic by George Anescu
	static const uchar saucHex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	uchar digest[20];
	GetHash(digest);
	
	for (int i=0; i != 20; ++i)
	{
		*(szReport+(i*2)) = saucHex[digest[i] >> 4];
		*(szReport+(i*2)+1) = saucHex[digest[i] & 0xF];
	}
}

// Get the raw message digest (20 bytes long)
void SHA1::GetHash(uchar *puDest) const throw()
{
	assert(finalized);
	assert(puDest != 0);
	
	memcpy(puDest, m_state, 20);
}
