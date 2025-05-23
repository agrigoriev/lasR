% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/internals.R
\name{install_cmd_tools}
\alias{install_cmd_tools}
\title{Use some lasR features from a terminal}
\usage{
install_cmd_tools()
}
\description{
Install the required files to be able to run some simple lasR commands from a terminal.
Working in a terminal is easier for simple tasks but it is not possible to build
complex pipelines this way. Examples of some possible commands:

\if{html}{\out{<div class="sourceCode">}}\preformatted{lasr help
lasr index -i /path/to/folder
lasr vpc -i /path/to/folder
lasr info -i /path/to/file.las
lasr chm -i /path/to/folder -o /path/to/chm.tif -res 1 -ncores 8
lasr dtm -i /path/to/folder -o /path/to/dtm.tif -res 0.5 -ncores 8
}\if{html}{\out{</div>}}
}
