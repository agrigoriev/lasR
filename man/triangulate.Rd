% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/stages.R
\name{triangulate}
\alias{triangulate}
\title{Delaunay triangulation}
\usage{
triangulate(max_edge = 0, filter = "", ofile = "", use_attribute = "Z")
}
\arguments{
\item{max_edge}{numeric. Maximum edge length of a triangle in the Delaunay triangulation. If a
triangle has an edge length greater than this value, it will be removed. If max_edge = 0, no trimming
is done (see examples).}

\item{filter}{the 'filter' argument allows filtering of the point-cloud to work with points of
interest. For a given stage when a filter is applied, only the points that meet the criteria are processed.
The most common strings are \verb{Classification == 2"}, \code{"Z > 2"}, \code{"Intensity < 100"}. For more details see
\link{filters}.}

\item{ofile}{character. Full outputs are always stored on disk. If \code{ofile = ""}
then the stage will not store the result on disk and will return nothing. It will however
hold partial output results temporarily in memory. This is useful for stage that are only
intermediate stage.}

\item{use_attribute}{character. Specifies the attribute to use for the operation, with "Z" (the coordinate) as the default.
Alternatively, this can be the name of any other attribute, such as "Intensity", "gpstime", "ReturnNumber", or "HAG", if it exists.
Note: The function does not fail if the specified attribute does not exist in the point cloud.
For example, if "Intensity" is requested but not present, or "HAG" is specified but unavailable,
the internal engine will return 0 for the missing attribute.}
}
\value{
This stage produces a vector. The path provided to `ofile` is expected to be `.gpkg` or
any other format supported by GDAL. Vector stages may produce geometries with Z coordinates.
Thus, it is discouraged to store them in formats with no 3D support, such as shapefiles.
}
\description{
2.5D Delaunay triangulation. Can be used to build a DTM, a CHM, normalize a point cloud, or any other
application. This stage is typically used as an intermediate process without an output file.
This stage does not modify the point cloud.
}
\examples{
f <- system.file("extdata", "Topography.las", package="lasR")
read <- reader()
tri1 <- triangulate(25, filter = keep_ground(), ofile = tempgpkg())
tri2 <- triangulate(ofile = tempgpkg())
pipeline <- read + tri1 + tri2
ans <- exec(pipeline, on = f)
#plot(ans[[1]])
#plot(ans[[2]])
}
