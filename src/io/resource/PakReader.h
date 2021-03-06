/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARX_IO_RESOURCE_PAKREADER_H
#define ARX_IO_RESOURCE_PAKREADER_H

#include <vector>
#include <iostream>

#include <boost/noncopyable.hpp>

#include "io/resource/PakEntry.h"
#include "io/resource/ResourcePath.h"
#include "platform/Flags.h"

namespace fs { class path; }

enum Whence {
	SeekSet,
	SeekCur,
	SeekEnd
};

class PakFileHandle : private boost::noncopyable  {
	
public:
	
	virtual size_t read(void * buf, size_t size) = 0;
	
	virtual int seek(Whence whence, int offset) = 0;
	
	virtual size_t tell() = 0;
	
	virtual ~PakFileHandle() { }
	
};

class PakReader : public PakDirectory {
	
public:
	
	enum ReleaseType {
		Demo     = (1<<0),
		FullGame = (1<<1),
		Unknown  = (1<<2)
	};
	DECLARE_FLAGS(ReleaseType, ReleaseFlags)
	
	inline PakReader() : release(0) { }
	~PakReader();
	
	void removeFile(const res::path & name);
	
	bool addFiles(const fs::path & path, const res::path & mount = res::path());
	
	bool addArchive(const fs::path & pakfile);
	void clear();
	
	bool read(const res::path & name, void * buf);
	char * readAlloc(const res::path & name , size_t & size);
	
	PakFileHandle * open(const res::path & name);
	
	inline ReleaseFlags getReleaseType() { return release; }
	
private:
	
	ReleaseFlags release;
	std::vector<std::istream *> paks;
	
	bool addFiles(PakDirectory * dir, const fs::path & path);
	bool addFile(PakDirectory * dir, const fs::path & path, const std::string & name);
	
};

DECLARE_FLAGS_OPERATORS(PakReader::ReleaseFlags)

extern PakReader * resources;

#endif // ARX_IO_RESOURCE_PAKREADER_H
