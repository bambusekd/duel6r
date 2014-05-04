/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DUEL6_LOADER_H
#define DUEL6_LOADER_H

#include <vector>
#include "Bonus.h"

#define D6_MALLOC(t,s)      (t *) MY_Alloc (sizeof (t) * (s))

//#define D6_RENDER_BACKS

#define D6_ANM_F_NOTHING    0x00
#define D6_ANM_F_BLOCK      0x01
#define D6_ANM_F_WATER      0x02
#define D6_ANM_F_FRONT      0x03
#define D6_ANM_F_BACK       0x04
#define D6_ANM_F_FRBC       0x05
#define D6_ANM_F_3FRONT     0x06
#define D6_ANM_F_3BACK      0x07
#define D6_ANM_F_WFALL      0x08

#define D6_FLAG_NONE        0x00
#define D6_FLAG_FLOW        0x01

namespace Duel6
{
	struct d6VERTEX
	{
		Float32 X;
		Float32 Y;
		Float32 Z;
		Float32 U;
		Float32 V;
		Int32 Flags;
	};

	struct d6FACE
	{
		Int32 NowTex;
		Int32 MinTex;
		Int32 MaxTex;
	};

	struct d6LEVEL
	{
		Int32 SizeX;
		Int32 SizeY;
		Int32 Size;
		Uint16 *Data;
	};

	struct d6ANM
	{
		std::vector<GLuint> textures;
		Float32 Wait;
		Int32 *Znak;
		Int32 *Anim;
	};

	struct d6WORLD
	{
		int         Blocks;
		int         Sprites;
		int         Waters;
		int         Faces;
		d6VERTEX    *Vertex;
		d6FACE      *Face;
		d6LEVEL     Level;
		d6ANM       Anm;
	};

	void LOADER_LoadWorld(const std::string& path, d6WORLD *world, bool mirror, std::vector<Bonus>& bonuses);
}

#endif