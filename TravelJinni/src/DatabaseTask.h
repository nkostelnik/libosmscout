#ifndef DATABASETASK_H
#define DATABASETASK_H

/*
  TravelJinni - Openstreetmap offline viewer
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

#include <Lum/Features.h>

#if defined(LUM_HAVE_LIB_CAIRO)
  #include <Lum/OS/Cairo/Bitmap.h>
  #include <Lum/OS/Cairo/DrawInfo.h>
#endif

#include <Lum/OS/Bitmap.h>
#include <Lum/OS/Thread.h>

#include <osmscout/Database.h>
#include <osmscout/Router.h>
#include <osmscout/RoutePostprocessor.h>
#include <osmscout/StyleConfig.h>
#include <osmscout/TypeConfig.h>

#include <osmscout/MapPainterCairo.h>

struct Job
{
  double                  lon;
  double                  lat;
  osmscout::Magnification magnification;
  size_t                  width;
  size_t                  height;
};

class DatabaseTask : public Lum::OS::Thread
{
private:
  osmscout::Database        *database;
  osmscout::StyleConfig     *styleConfig;
  osmscout::MapData         data;
  osmscout::RouterRef       router;
  osmscout::FastestPathRoutingProfile routingProfile;
  osmscout::RoutePostprocessor postprocessor;
  Lum::OS::Condition        condition;
  osmscout::MapPainterCairo painter;

  mutable Lum::OS::Mutex    mutex;
  bool                      finish;
  Job                       *newJob;
  Job                       *currentJob;
  Job                       *finishedJob;
  Lum::Model::Action        *jobFinishedAction;
  cairo_surface_t           *currentSurface;
  cairo_t                   *currentCairo;
  size_t                    currentWidth,currentHeight;
  double                    currentLon,currentLat;
  osmscout::Magnification   currentMagnification;
  cairo_surface_t           *finishedSurface;
  cairo_t                   *finishedCairo;
  size_t                    finishedWidth,finishedHeight;
  double                    finishedLon,finishedLat;
  osmscout::Magnification   finishedMagnification;

private:
  void SignalRedraw();
  void GetCarSpeedTable(std::map<std::string,double>& map);


public:
  DatabaseTask(osmscout::Database* database,
               const osmscout::RouterRef& router,
               Lum::Model::Action* jobFinishedAction);

  void Run();
  void Finish();

  bool Open(const std::wstring& path);
  bool IsOpen() const;
  void Close();

  void FlushCache();

  bool LoadStyleConfig(const std::wstring& filename,
                       osmscout::StyleConfig*& styleConfig);
  void SetStyle(osmscout::StyleConfig* styleConfig);


  bool GetBoundingBox(double& minLat,double& minLon,
                      double& maxLat,double& maxLon) const;

  bool GetWayByOffset(osmscout::FileOffset offset,
                      osmscout::WayRef& way) const;

  bool SearchForLocations(const osmscout::LocationSearch& search,
                          osmscout::LocationSearchResult& result) const;

  bool CalculateRoute(const osmscout::ObjectFileRef& startObject,
                      size_t startNodeIndex,
                      const osmscout::ObjectFileRef& targetObject,
                      size_t targetNodeIndex,
                      osmscout::RouteData& route);
  bool TransformRouteDataToRouteDescription(const osmscout::RouteData& data,
                                            osmscout::RouteDescription& description,
                                            const std::string& start,
                                            const std::string& target);
  bool PostprocessRouteDescription(osmscout::RouteDescription& description);
  bool TransformRouteDataToWay(const osmscout::RouteData& data,
                               osmscout::Way& way);
  void ClearRoute();
  void AddRoute(const osmscout::Way& way);

  void PostJob(Job *job);

  bool DrawResult(Lum::OS::Window* window,
                  Lum::OS::DrawInfo* draw,
                  int x, int y,
                  size_t width, size_t height,
                  double lon, double lat,
                  const osmscout::Magnification& magnification,
                  osmscout::Projection& projection);
};

#endif
