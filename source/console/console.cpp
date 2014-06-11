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

/*
Projekt: Konzole
Popis: Hlavni funkce
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ConsoleException.h"
#include "console.h"

///////////////////////////////////////////////////////
//                Zakladni funkce                    //
///////////////////////////////////////////////////////

namespace Duel6
{
	/*
	==================================================
	Inicializace/deinicializace konzole
	==================================================
	*/
	Console::Console(int flags)
	{
		visible = false;
		insert = false;
		curpos = 0;
		inputscroll = 0;
		this->flags = flags;

		histcnt = 0;
		histscroll = 0;
		font = nullptr;

		width = CON_DEF_WIDTH;
		show = CON_DEF_SHOWLAST;

		text.resize(CON_TEXT_SIZE);
		clear();

		print("==========================\n");
		print(CON_Lang("Console -- author O.Danek\n"));
		print(CON_Format(CON_Lang("Version {0}\n")) << CON_VERSION);
		print("==========================\n");
	}

	Console::~Console()
	{
		vars.clear();
		cmds.clear();
		aliases.clear();
	}

	/*
	==================================================
	Vyprazdneni textoveho bufferu
	==================================================
	*/
	void Console::clear()
	{
		bufpos = 0;
		scroll = 0;
		buffull = false;
		memset(&text[0], '\n', CON_TEXT_SIZE);
	}

	/*
	==================================================
	Formatovany tisk do konzoly
	V infoproc by se nemelo s konzolou nic delat
	==================================================
	*/
	void Console::print(const std::string& str)
	{
		for (size_t pos = 0; pos < str.length(); ++pos)
		{
			char tx = str[pos];

			switch (tx)
			{
			case '\n':
				text[bufpos++] = tx;
				break;
			case '\t':
				for (int i = 0; i < CON_TAB_WIDTH; i++, bufpos++)
				{
					text[bufpos % CON_TEXT_SIZE] = ' ';
				}
				break;
			default:
				if (tx >= ' ')
				{
					text[bufpos++] = tx;
				}
				break;
			}

			if (bufpos >= CON_TEXT_SIZE)
			{
				bufpos -= CON_TEXT_SIZE;
				buffull = true;
			}
		}

		scroll = 0;
	}

	///////////////////////////////////////////////////////
	//                  Registrace                       //
	///////////////////////////////////////////////////////

	/*
	==================================================
	Kontrola pripustnosti jmena
	==================================================
	*/
	void Console::verifyRegistration(const std::string& proc, const std::string& name, bool isNull)
	{
		if (name.empty())
		{
			D6_THROW(ConsoleException, CON_Format(CON_Lang("{0} : cannot register empty name")) << proc);
		}

		if (findCommand(name) != nullptr || findVar(name) != nullptr || findAlias(name) != nullptr)
		{
			D6_THROW(ConsoleException, CON_Format(CON_Lang("{0} : name \"{1}\" is already registered")) << proc << name);
		}

		if (isNull)
		{
			D6_THROW(ConsoleException, CON_Format(CON_Lang("{0} : attempt to register \"{1}\" with null pointer")) << proc << name);
		}
	}

	/*
	==================================================
	Registrovani prikazu
	==================================================
	*/
	void Console::registerCommand(const std::string& name, Command command)
	{
		verifyRegistration(CON_Lang("Command registration"), name, !command);

		CommandRecord newCommand(name, command);

		// Zaradi novy prikaz tak, aby byly setridene podle abecedy
		size_t position = 0;
		for (const CommandRecord& command : cmds)
		{
			if (command.getName().compare(name) > 0)
			{
				break;
			}
			++position;
		}

		cmds.insert(cmds.begin() + position, newCommand);

		if (flags & CON_F_REG_INFO)
		{
			print(CON_Format(CON_Lang("Command registration: \"{0}\" has been successful\n")) << name);
		}
	}

	/*
	==================================================
	Registrace aliasu
	==================================================
	*/
	void Console::registerAlias(const std::string& name, const std::string& cmd)
	{
		AliasRecord* a = findAlias(name);

		if (a == nullptr)
		{
			verifyRegistration(CON_Lang("Alias registration"), name, false);

			AliasRecord newAlias(name, cmd);

			// Zaradi novy alias tak, aby byly setridene podle abecedy
			size_t position = 0;
			for (const AliasRecord& alias : aliases)
			{
				if (alias.getName().compare(name) > 0)
				{
					break;
				}
				++position;
			}

			aliases.insert(aliases.begin() + position, newAlias);
		}
		else
		{
			a->setCommand(cmd);
		}

		if (flags & CON_F_REG_INFO)
		{
			print(CON_Format(CON_Lang("Alias registration: \"{0}\" as \"{1}\" has been successful\n")) << name << cmd);
		}
	}

	/*
	==================================================
	Vraceni ukazatele na prikaz
	==================================================
	*/
	Console::CommandRecord* Console::findCommand(const std::string& name)
	{
		for (CommandRecord& command : cmds)
		{
			if (command.getName() == name)
			{
				return &command;
			}
		}

		return nullptr;
	}

	/*
	==================================================
	Vraci ukazatel na alias s danym jmenem
	==================================================
	*/
	Console::AliasRecord* Console::findAlias(const std::string& name)
	{
		for (AliasRecord& alias : aliases)
		{
			if (alias.getName() == name)
			{
				return &alias;
			}
		}

		return nullptr;
	}

	/*
	==================================================
	Nastaveni ukazatele na font
	==================================================
	*/
	void Console::setFont(const Uint8* p)
	{
		font = p;
	}

	/*
	==================================================
	Nastaveni kolik predchozich radku bude zobrazeno
	==================================================
	*/
	void Console::setLast(int sl)
	{
		show = sl > 1 ? sl : 2;
	}

	/*
	==================================================
	Vraci ukazatel na textovy buffer konzole
	==================================================
	*/
	const Uint8* Console::getText(Size& bufPos, bool& bufFull) const
	{
		bufPos = bufpos;
		bufFull = buffull;
		return &text[0];
	}
}