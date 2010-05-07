#ifndef OSMSCOUT_RELATION_H
#define OSMSCOUT_RELATION_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2010  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/Point.h>

#include <osmscout/FileScanner.h>
#include <osmscout/FileWriter.h>
#include <osmscout/TypeConfig.h>

namespace osmscout {

  /**
    Representation of an relation
    */
  class Relation
  {
  public:
    struct Role
    {
      // Common flags
      const static uint16_t hasName         = 1 <<  0; //! We have a name
      const static uint16_t hasRef          = 1 <<  1; //! We have reference name

      // Way flags
      const static uint16_t hasLayer        = 1 <<  2; //! We have optional layer information

      TypeId             type;
      uint16_t           flags;
      std::string        role;
      std::string        name;
      std::string        ref;
      int8_t             layer;
      std::vector<Point> nodes;
    };

  public:
    const static uint16_t isArea   = 1 << 0; //! We are an area
    const static uint16_t hasTags  = 1 << 1; //! We have additional tags stored on disk

  public:
    Id                id;
    TypeId            type;
    std::string       relType;
    uint16_t          flags;
    std::vector<Tag>  tags;
    std::vector<Role> roles;

  public:
    inline Relation()
    : type(typeIgnore)
    {
      // no code
    }

    inline bool IsArea() const
    {
      return flags & isArea;
    }

    bool GetCenter(double& lat, double& lon) const;

    bool Read(FileScanner& scanner);
    bool Write(FileWriter& writer) const;
  };
}

#endif