#include "writelax.h"
#include "macros.h"

#include "lasreader.hpp"
#include "laszip_decompress_selective_v3.hpp"
#include "lasindex.hpp"
#include "lasquadtree.hpp"

void LASRlaxwriter::set_input_file(std::string file)
{
  // Initialize las objects
  const char* filechar = const_cast<char*>(file.c_str());
  LASreadOpener lasreadopener;
  lasreadopener.set_file_name(filechar);
  LASreader* lasreader = lasreadopener.open();

  if (!lasreader)
  {
    last_error = "LASlib internal error";
    return;
  }

  lasreadopener.set_decompress_selective(LASZIP_DECOMPRESS_SELECTIVE_CHANNEL_RETURNS_XY);

  // setup the quadtree
  LASquadtree* lasquadtree = new LASquadtree;

  float w = lasreader->header.max_x - lasreader->header.min_x;
  float h = lasreader->header.max_y - lasreader->header.min_y;
  F32 t;

  if ((w < 1000) && (h < 1000))
    t = 10.0;
  else if ((w < 10000) && (h < 10000))
    t = 100.0;
  else if ((w < 100000) && (h < 100000))
    t = 1000.0;
  else if ((w < 1000000) && (h < 1000000))
    t = 10000.0;
  else
    t = 100000.0;

  lasquadtree->setup(lasreader->header.min_x, lasreader->header.max_x, lasreader->header.min_y, lasreader->header.max_y, t);

  LASindex lasindex;
  lasindex.prepare(lasquadtree, 1000);

  progress->reset();
  progress->set_prefix("Write LAX");
  progress->set_total(lasreader->header.number_of_point_records);

  while (lasreader->read_point())
  {
    lasindex.add(lasreader->point.get_x(), lasreader->point.get_y(), (U32)(lasreader->p_count-1));
    (*progress)++;
    progress->show();
  }

  lasreader->close();
  delete lasreader;

  lasindex.complete(100000, -20, verbose);
  lasindex.write(lasreadopener.get_file_name());

  progress->done();

  return;
}