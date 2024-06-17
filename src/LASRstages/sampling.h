#ifndef SAMPLINGVOXEL_H
#define SAMPLINGVOXEL_H

#include "Stage.h"

#include <algorithm>
#include <random>

class LASRsampling : public Stage
{
public:
  void shuffle(std::vector<int>& x, int shuffle_size)
  {
    std::mt19937 rng(0);

    if (shuffle_size > 0)
    {
      int s = x.size();
      for (int i = 0; i < s; i += shuffle_size/2)
      {
        int start = std::max(0, i - shuffle_size / 2);
        int end = std::min(s, i + shuffle_size / 2 + 1);
        std::shuffle(x.begin() + start, x.begin() + end, rng);
      }
    }
  };
};

class LASRsamplingpoisson : public LASRsampling
{
public:
  LASRsamplingpoisson(double xmin, double ymin, double xmax, double ymax, double distance, int shuffle_size);
  bool process(LAS*& las) override;
  double need_buffer() const override { return distance; }
  std::string get_name() const override { return "poisson_sampling"; }

  // multi-threading
  bool is_parallelizable() const override { return true; };
  LASRsamplingpoisson* clone() const override { return new LASRsamplingpoisson(*this); };

private:
  double distance;
  int shuffle_size;
};

class LASRsamplingvoxels : public LASRsampling
{
public:
  LASRsamplingvoxels(double xmin, double ymin, double xmax, double ymax, double res, int shuffle_size);
  bool process(LAS*& las) override;
  double need_buffer() const override { return res; }
  std::string get_name() const override { return "voxel_sampling"; }

  // multi-threading
  bool is_parallelizable() const override { return true; };
  LASRsamplingvoxels* clone() const override { return new LASRsamplingvoxels(*this); };

private:
  double res;
  int shuffle_size;
};

class LASRsamplingpixels : public LASRsampling
{
public:
  LASRsamplingpixels(double xmin, double ymin, double xmax, double ymax, double res, int shuffle_size);
  bool process(LAS*& las) override;
  double need_buffer() const override { return res; }
  std::string get_name() const override { return "pixel_sampling"; }

  // multi-threading
  bool is_parallelizable() const override { return true; };
  LASRsamplingpixels* clone() const override { return new LASRsamplingpixels(*this); };

private:
  double res;
  int shuffle_size;
};

#endif