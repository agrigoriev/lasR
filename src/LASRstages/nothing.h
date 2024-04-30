#ifndef NOTHING_H
#define NOTHING_H

#include "Stage.h"

class LASRnothing : public Stage
{
public:
  LASRnothing(bool read, bool stream, bool loop);
  std::string get_name() const override { return "nothing"; };
  bool process(LAS*& las) override;
  bool is_streamable() const override { return streamable; };
  bool need_points() const override { return read_points; };

  // multi-threading
  LASRnothing* clone() const override { return new LASRnothing(*this); };

  private:
    bool streamable;
    bool read_points;
    bool loop;
};
#endif