/*
  ReaderScannerPerformance - a test program for libosmscout
  Copyright (C) 2009  Tim Teulings

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <cstdlib>
#include <iostream>

#include <osmscout/FileReader.h>
#include <osmscout/FileScanner.h>
#include <osmscout/Way.h>
#include <osmscout/WayIndex.h>
#include <osmscout/Util.h>

/**
  Sequentially read the ways.dat file in the current directory to collect
  way ids. Then take a subset of ids randomly and call the IdIndex.
  Measure execution time.

  Call this program repeately to avoid different timing because of OS file caching.
*/

#define QUERY_COUNT 10000000

int main(int argc, char* argv[])
{
  std::vector<Id> ids;
  std::vector<Id> queries;
  std::string     filename="ways.dat";
  long            filesize=0;
  size_t          readerWayCount;

  if (!GetFileSize(filename,filesize)) {
    std::cerr << "Cannot get file size of file '" << filename << "'!" << std::endl;
    return 1;
  }

  StopClock readerTimer;

  FileReader reader;

  if (!reader.Open(filename)) {
    std::cerr << "Cannot open file '" << filename << "'!" << std::endl;
    return 1;
  }

  if (!reader.ReadPageToBuffer(0,filesize)) {
    std::cerr << "Cannot read file '" << filename << "' into memory!" << std::endl;
    return 1;
  }

  std::cout << "Start reading files using FileReader..." << std::endl;

  readerWayCount=0;
  while (!reader.HasError()) {
    Way way;

    if (way.Read(reader)) {
      ids.push_back(way.id);
      readerWayCount++;
    }
  }

  reader.Close();

  readerTimer.Stop();

  std::cout << "Reading " << readerWayCount << " ways via FileReader took " << readerTimer << std::endl;

  queries.reserve(QUERY_COUNT);

  for (size_t i=0; i<QUERY_COUNT; i++) {
    queries.push_back(ids[(int)(QUERY_COUNT*rand()/(RAND_MAX+1.0))]);
  }


  WayIndex wayIndex;

  StopClock indexTimer;

  if (!wayIndex.LoadWayIndex(".")) {
    std::cerr << "Cannot open way index file!" << std::endl;
    return 1;
  }

  for (size_t i=0; i<queries.size(); i++) {
    std::set<Id>             ids;
    std::list<WayIndexEntry> entries;

    ids.insert(queries[i]);

    wayIndex.GetWayIndexEntries(ids,entries);

    if (entries.size()!=1) {
      std::cerr << "Cannot read way id " << queries[i] << " from index!" << std::endl;
    }
  }

  indexTimer.Stop();

  std::cout << "Reading " << queries.size() << " random way ids from index took " << indexTimer << std::endl;

  return 0;
}