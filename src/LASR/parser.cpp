#include "pipeline.h"

#include "addattribute.h"
#include "boundaries.h"
#include "localmaximum.h"
#include "noiseivf.h"
#include "nothing.h"
#include "pitfill.h"
#include "rasterize.h"
#include "sampling.h"
#include "readlas.h"
#include "regiongrowing.h"
#include "summary.h"
#include "triangulate.h"
#include "triangulatedtransformer.h"
#include "writelas.h"
#include "writelax.h"

#ifdef USING_R
#include "callback.h"
#include "aggregate.h"
#endif


#ifdef USING_R
bool contains_element(SEXP list, const char *str)
{
  SEXP names = Rf_getAttrib(list, R_NamesSymbol);
  for (int i = 0 ; i < Rf_length(list) ; i++) { if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0) return true; }
  return false;
}

SEXP get_element(SEXP list, const char *str)
{
  SEXP elmt = R_NilValue;
  SEXP names = Rf_getAttrib(list, R_NamesSymbol);
  for (int i = 0 ; i < Rf_length(list) ; i++) { if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0)  { elmt = VECTOR_ELT(list, i); break; }}
  if (Rf_isNull(elmt)) error("element '%s' not found", str);
  return elmt;
}

bool get_element_as_bool(SEXP list, const char *str) { return LOGICAL(get_element(list, str))[0] != 0; }
int get_element_as_int(SEXP list, const char *str) { return INTEGER(get_element(list, str))[0]; }
double get_element_as_double(SEXP list, const char *str) { return REAL(get_element(list, str))[0]; }
std::string get_element_as_string(SEXP list, const char *str) { return std::string(CHAR(STRING_ELT(get_element(list, str), 0))); }
std::vector<int> get_element_as_vint(SEXP list, const char *str)
{
  SEXP res = get_element(list, str);
  std::vector<int> ans(Rf_length(res));
  for (int i = 0 ; i < Rf_length(res) ; ++i) ans[i] = INTEGER(res)[i];
  return ans;
}
std::vector<double> get_element_as_vdouble(SEXP list, const char *str)
{
  SEXP res = get_element(list, str);
  std::vector<double> ans(Rf_length(res));
  for (int i = 0 ; i < Rf_length(res) ; ++i) ans[i] = REAL(res)[i];
  return ans;
}
#endif

bool Pipeline::parse(const SEXP sexpargs, LAScatalog& lascatalog)
{
  double xmin = lascatalog.xmin;
  double ymin = lascatalog.ymin;
  double xmax = lascatalog.xmax;
  double ymax = lascatalog.ymax;

  parsed = false;
  pipeline.clear();

  for (auto i = 0; i < Rf_length(sexpargs); ++i)
  {
    SEXP algorithm = VECTOR_ELT(sexpargs, i);

    std::string name   = get_element_as_string(algorithm, "algoname");
    std::string uid    = get_element_as_string(algorithm, "uid");
    std::string filter = get_element_as_string(algorithm, "filter");
    std::string output = get_element_as_string(algorithm, "output");

    if (name == "reader")
    {
      if (i != 0)
      {
        error("The reader must alway be the first stage of the pipeline.");
        return false;
      }

      auto v = std::make_shared<LASRlasreader>();
      pipeline.push_back(v);

      // Special treatment of the reader to find the potential queries in the catalog
      if (contains_element(algorithm, "xcenter"))
      {
        std::vector<double> xcenter = get_element_as_vdouble(algorithm, "xcenter");
        std::vector<double> ycenter = get_element_as_vdouble(algorithm, "ycenter");
        std::vector<double> radius = get_element_as_vdouble(algorithm, "radius");
        for (auto j = 0 ; j <  xcenter.size() ; ++j) lascatalog.add_query(xcenter[j], ycenter[j], radius[j]);
      }

      if (contains_element(algorithm, "xmin"))
      {
        std::vector<double> xmin = get_element_as_vdouble(algorithm, "xmin");
        std::vector<double> ymin = get_element_as_vdouble(algorithm, "ymin");
        std::vector<double> xmax = get_element_as_vdouble(algorithm, "xmax");
        std::vector<double> ymax = get_element_as_vdouble(algorithm, "ymax");
        for (auto j = 0 ; j <  xmin.size() ; ++j) lascatalog.add_query(xmin[j], ymin[j], xmax[j], ymax[j]);
      }
    }
    else if (name == "rasterize")
    {
      double res = get_element_as_double(algorithm, "res");

      if (!contains_element(algorithm, "connect"))
      {
        std::vector<int> methods = get_element_as_vint(algorithm, "method");
        auto v = std::make_shared<LASRrasterize>(xmin, ymin, xmax, ymax, res, methods);
        pipeline.push_back(v);
      }
      else
      {
        std::string uid = get_element_as_string(algorithm, "connect");
        auto it = std::find_if(pipeline.begin(), pipeline.end(), [&uid](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid; });
        if (it == pipeline.end()) { error("Cannot find algorithm with this uid"); return false; }

        LASRtriangulate* p = dynamic_cast<LASRtriangulate*>(it->get());
        if (p)
        {
          auto v = std::make_shared<LASRrasterize>(xmin, ymin, xmax, ymax, res, p);
          pipeline.push_back(v);
        }
        else
        {
          error("Incompatible algorithm combination for rasterize");
          return false;
        }
      }
    }
    else if (name == "local_maximum")
    {
      double ws = get_element_as_double(algorithm, "ws");
      double min_height = get_element_as_double(algorithm, "min_height");
      std::string use_attribute = get_element_as_string(algorithm, "use_attribute");
      auto v = std::make_shared<LASRlocalmaximum>(xmin, ymin, xmax, ymax, ws, min_height, use_attribute);
      pipeline.push_back(v);
    }
    else if (name == "summarise")
    {
      double zwbin = get_element_as_double(algorithm, "zwbin");
      double iwbin = get_element_as_double(algorithm, "iwbin");
      auto v = std::make_shared<LASRsummary>(xmin, ymin, xmax, ymax, zwbin, iwbin);
      pipeline.push_back(v);
    }
    else if (name == "triangulate")
    {
      double max_edge = get_element_as_double(algorithm, "max_edge");
      std::string use_attribute = get_element_as_string(algorithm, "use_attribute");
      auto v = std::make_shared<LASRtriangulate>(xmin, ymin, xmax, ymax, max_edge, use_attribute);
      pipeline.push_back(v);
    }
    else if (name == "write_las")
    {
      bool keep_buffer = get_element_as_bool(algorithm, "keep_buffer");
      auto v = std::make_shared<LASRlaswriter>(xmin, xmax, ymin, ymax, keep_buffer);
      pipeline.push_back(v);
    }
    else if (name == "write_lax")
    {
      auto v = std::make_shared<LASRlaxwriter>();
      pipeline.push_back(v);
    }
    else if (name == "transform_with_triangulation")
    {
      std::string uid = get_element_as_string(algorithm, "connect");
      std::string op = get_element_as_string(algorithm, "operator");
      std::string attr = get_element_as_string(algorithm, "store_in_attribute");
      auto it = std::find_if(pipeline.begin(), pipeline.end(), [&uid](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid; });
      if (it == pipeline.end()) { error("Cannot find algorithm with this uid"); return false; }

      LASRtriangulate* p = dynamic_cast<LASRtriangulate*>(it->get());
      if (p)
      {
        auto v = std::make_shared<LASRtriangulatedTransformer>(xmin, ymin, xmax, ymax, p, op, attr);
        pipeline.push_back(v);
      }
      else
      {
        error("Incompatible algorithm combination for 'rasterize'");
        return false;
      }
    }
    else if (name == "pit_fill")
    {
      std::string uid = get_element_as_string(algorithm, "connect");
      int lap_size = get_element_as_int(algorithm, "lap_size");
      float thr_lap = (float)get_element_as_double(algorithm, "thr_lap");
      float thr_spk = (float)get_element_as_double(algorithm, "thr_spk");
      int med_size = get_element_as_int(algorithm, "med_size");
      int dil_radius = get_element_as_int(algorithm, "dil_radius");

      auto it = std::find_if(pipeline.begin(), pipeline.end(), [&uid](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid; });
      if (it == pipeline.end()) { error("Cannot find algorithm with this uid"); return false; }

      LASRrasterize * p = dynamic_cast<LASRrasterize*>(it->get());
      if (p)
      {
        auto v = std::make_shared<LASRpitfill>(xmin, ymin, xmax, ymax, lap_size, thr_lap, thr_spk, med_size, dil_radius, p);
        pipeline.push_back(v);
      }
      else
      {
        error("Incompatible algorithm combination for 'rasterize'");
        return false;
      }
    }
    else if (name  == "sampling_voxel")
    {
      double res = get_element_as_double(algorithm, "res");
      auto v = std::make_shared<LASRsamplingvoxels>(xmin, ymin, xmax, ymax, res);
      pipeline.push_back(v);
    }
    else if (name  == "sampling_pixel")
    {
      double res = get_element_as_double(algorithm, "res");
      auto v = std::make_shared<LASRsamplingpixels>(xmin, ymin, xmax, ymax, res);
      pipeline.push_back(v);
    }
    else if (name  == "region_growing")
    {
      double th_tree = get_element_as_double(algorithm, "th_tree");
      double th_seed = get_element_as_double(algorithm, "th_seed");
      double th_cr = get_element_as_double(algorithm, "th_cr");
      double max_cr = get_element_as_double(algorithm, "max_cr");

      std::string uid1 = get_element_as_string(algorithm, "connect1");
      std::string uid2 = get_element_as_string(algorithm, "connect2");
      auto it1 = std::find_if(pipeline.begin(), pipeline.end(), [&uid1](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid1; });
      if (it1 == pipeline.end()) { error("Cannot find algorithm with this uid");  return false; }
      auto it2 = std::find_if(pipeline.begin(), pipeline.end(), [&uid2](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid2; });
      if (it2 == pipeline.end()) { error("Cannot find algorithm with this uid");  return false; }

      LASRlocalmaximum* p = dynamic_cast<LASRlocalmaximum*>(it1->get());
      LASRalgorithmRaster* q = dynamic_cast<LASRalgorithmRaster*>(it2->get());
      if (p && q)
      {
        auto v = std::make_shared<LASRregiongrowing>(xmin, ymin, xmax, ymax,th_tree, th_seed, th_cr, max_cr, q, p);
        pipeline.push_back(v);
      }
      else
      {
        error("Incompatible algorithm combination for 'region_growing'");
        return false;
      }
    }
    else if (name == "boundaries")
    {
      LASRtriangulate* p = nullptr;
      if (contains_element(algorithm, "connect"))
      {
        std::string uid = get_element_as_string(algorithm, "connect");
        auto it = std::find_if(pipeline.begin(), pipeline.end(), [&uid](const std::shared_ptr<LASRalgorithm>& obj) { return obj->get_uid() == uid; });
        if (it == pipeline.end()) { error("Cannot find algorithm with this uid"); return false; }
        p = dynamic_cast<LASRtriangulate*>(it->get());
        if (p == nullptr)
        {
          error("Incompatible algorithm combination for 'boundaries'");
          return false;
        }
      }

      auto v = std::make_shared<LASRboundaries>(xmin, ymin, xmax, ymax, p);
      pipeline.push_back(v);
    }
    else if (name == "classify_isolated_points")
    {
      double res = get_element_as_double(algorithm, "res");
      int n = get_element_as_int(algorithm, "filter");
      int classification = get_element_as_int(algorithm, "class");
      auto v = std::make_shared<LASRnoiseivf>(xmin, ymin, xmax, ymax, res, n, classification);
      pipeline.push_back(v);
    }
    else if (name == "add_extrabytes")
    {
      std::string data_type = get_element_as_string(algorithm, "data_type");
      std::string name = get_element_as_string(algorithm, "name");
      std::string desc = get_element_as_string(algorithm, "description");
      double scale = get_element_as_double(algorithm, "scale");
      double offset = get_element_as_double(algorithm, "offset");
      auto v = std::make_shared<LASRaddattribute>(data_type, name, desc, scale, offset);
      pipeline.push_back(v);
    }
    else if (name == "nothing")
    {
      auto v = std::make_shared<LASRnothing>();
      pipeline.push_back(v);
    }
#ifdef USING_R
    else if (name == "aggregate")
    {
      SEXP call = get_element(algorithm, "call");
      SEXP env = get_element(algorithm, "env");
      double res = get_element_as_double(algorithm, "res");
      auto v = std::make_shared<LASRaggregate>(xmin, ymin, xmax, ymax, res, call, env);
      pipeline.push_back(v);
    }
    else if (name  == "callback")
    {
      std::string expose = get_element_as_string(algorithm, "expose");
      bool modify = !get_element_as_bool(algorithm, "no_las_update");
      bool drop_buffer = get_element_as_bool(algorithm, "drop_buffer");
      SEXP fun = get_element(algorithm, "fun");
      SEXP args = get_element(algorithm, "args");
      auto v = std::make_shared<LASRcallback>(xmin, ymin, xmax, ymax, expose, fun, args, modify, drop_buffer);
      pipeline.push_back(v);
    }
#endif
    else
    {
      error("Unsupported algorithm");
      return false;
    }

    auto it = pipeline.back();
    it->set_uid(uid);
    it->set_filter(filter);
    it->set_output_file(output);
  }

  if (pipeline[0]->get_name() != "read_las")
  {
    last_error = "The pipeline must start with a readers";
    return false;
  }

  parsed = true;
  streamable = is_streamable();
  buffer = need_buffer();
  read_payload = need_points();

  for (auto&& stage : pipeline)
  {
    stage->set_ncpu(ncpu);
    stage->set_verbose(verbose);
  }

  return true;
}