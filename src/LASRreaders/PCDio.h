#ifndef PCDIO_H
#define PCDIO_H

#include "PointCloud.h"
#include "Chunk.h"

#include <string>
#include <vector>
#include <numeric>
#include <fstream>

class Progress;

class PCDio
{
public:
  PCDio();
  PCDio(Progress*);
  ~PCDio();
  bool open(const Chunk& chunk, std::vector<std::string> filters);
  bool open(const std::string& file);
  //bool create(const std::string& file);
  bool populate_header(Header* header);
  //bool init(const Header* header, const CRS& crs);
  bool read_point(Point* p);
  //bool write_point(Point* p);
  bool is_opened();
  void close();
  void reset_accessor();
  int64_t p_count();


private:
  Progress* progress;
  Header* header;
  std::ifstream istream;
  std::ofstream ostream;
  std::string line;
  std::string file;
  int64_t npoints;
  bool is_binary;
};

#endif
