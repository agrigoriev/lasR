% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/doc.R
\name{metric_engine}
\alias{metric_engine}
\title{Metric engine}
\description{
The metric engine is an internal tool that allow to derive any metric from a set of points by parsing a
string. It is used by \link{rasterize}, \link{summarise} as well as other functions. Each string
is composed of two parts separated by an underscore. The first part is the attribute on which the
metric must be computed (e.g., z, intensity, classification). The second part is the name of the
metric (e.g., \code{mean}, \code{sd}, \code{cv}). A string thus typically looks like \code{"z_max"}, \code{"intensity_min"},
\code{"z_mean"}, \code{"classification_mode"}. For more details see the sections 'Attribute' and 'Metrics'
respectively.
}
\details{
\strong{Be careful}: the engine supports any combination of \code{attribute_metric} strings. While they are
all computable, they are not all meaningful. For example, \code{c_mode} makes sense but not \code{z_mode}. Also,
all metrics are computed with 32-bit floating point accuracy, so \code{x_mean} or \code{y_sum} might be
slightly inaccurate, but anyway, these metrics are not supposed to be useful.
}
\section{Attribute}{

The available attributes are accessible via their name. Some standard attribute have a shortcut
by using a single letter: t - gpstime, a - angle, i - intensity, n - numberofreturns,
r - returnnumber, c - classification, u - userdata, p - pointsourceid, e - edgeofflightline,
d - scandirectionflag, R - red, G - green, B - blue, N - nir.\cr
\strong{Be careful} to the typos: attributes are non failing features. If the attribute does not exist \code{NaN}
is returned. Thus \code{intesity_mean} return \code{NaN} rather than failing.
}

\section{Metrics}{

The available metric names are: \code{count}, \code{max}, \code{min}, \code{mean}, \code{median}, \code{sum}, \code{sd}, \code{cv}, \code{pX} (percentile), \code{aboveX}, \code{mode},
\code{kurt} (kurtosis), \code{skew} (skewness).
Some metrics have an attribute + name + a parameter \code{X}, such as \code{pX} where \code{X} can be substituted by a number.
Here, \code{z_pX} represents the Xth percentile; for instance, \code{z_p95} signifies the 95th
percentile of z. \code{z_aboveX} corresponds to the percentage of points above \code{X} (sometimes called canopy cover).\cr\cr
It is possible to call a metric without the name of the attribute. In this case, z is the default. e.g. \code{mean} equals \code{z_mean}
}

\section{Extra attribute}{

The core attributes natively supported are x, y, z, classification, intensity, and so on. Some
point clouds have other may have other attributes. In this case, metrics can be derived the same way using
the names of the attributes. Be careful of typos. The attributes existance are not checked internally
because. For example, if a user requests: \code{ntensity_mean}, this could be a typo or the name of an
extra attribute. Because extra attribute are never failing, \code{ntensity_mean} will return \code{NaN}
rather than an error.
}

\examples{
metrics = c("z_max", "i_min", "r_mean", "n_median", "z_sd", "c_sd", "t_cv", "u_sum", "z_p95")
f <- system.file("extdata", "Example.las", package="lasR")
p <- summarise(metrics = metrics)
r <- rasterize(5, operators = metrics)
ans <- exec(p+r, on = f)
ans$summary$metrics
ans$rasterize
}
