lasR <img src="https://github.com/r-lidar/lasR/blob/main/man/figures/lasR200x231.png?raw=true" align="right"/>
======================================================================================================

![license](https://img.shields.io/badge/Licence-GPL--3-blue.svg)
![Lifecycle:Maturing](https://img.shields.io/badge/Lifecycle-Maturing-339999)
[![R-CMD-check](https://github.com/r-lidar/lasR/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/r-lidar/lasR/actions/workflows/R-CMD-check.yaml)
[![Codecov test coverage](https://codecov.io/gh/r-lidar/lasR/branch/main/graph/badge.svg)](https://app.codecov.io/gh/r-lidar/lasR?branch=main)

**R Package for Fast Airborne LiDAR Data Processing**

The `lasR` package (pronounced "laser") is a highly efficient software designed for professional use, enabling the creation and execution of complex processing pipelines on terabytes of airborne LiDAR data. It can read and write `.las` and `.laz` files, compute metrics using an area-based approach, generate digital canopy models, segment individual trees, thin point data, and handle collections of tiles with multicore processing. `lasR` offers a range of tools to process massive volumes of LiDAR data efficiently in a **production environment**.

- 📖 Start with the [tutorial](https://r-lidar.github.io/lasR/articles/tutorial.html) to learn how to use `lasR`.
- 💻 Install `lasR` in R with: `install.packages('lasR', repos = 'https://r-lidar.r-universe.dev')`.
- 💵 [Sponsor `lasR`](https://github.com/sponsors/Jean-Romain). It is free and open source, but requires time and effort to develop and maintain.

`lasR` **is not intended** to replace the [`lidR`](https://github.com/r-lidar/lidR) package. While `lidR` is tailored for academic research, `lasR` focuses on production scenarios, offering significantly higher efficiency for common tasks on large datasets. For more details, see the [comparison](https://r-lidar.github.io/lasR/articles/benchmarks.html).

## Installation

There are no current plans to release `lasR` on CRAN. Instead, it is hosted on `r-universe`:

```r
install.packages('lasR', repos = 'https://r-lidar.r-universe.dev')
```

Since `lasR` is not available on CRAN, users cannot rely on the CRAN versioning system or the RStudio update button to get the latest version. Instead, when `lasR` is loaded with `library(lasR)`, an internal routine checks for the latest version and notifies the user if an update is available. This approach allows for more frequent updates, ensuring users have access to the newest features and bug fixes without waiting for a formal release cycle.

```r
library(lasR)
#> lasR 0.1.3 is now available. You are using 0.1.1
#> install.packages('lasR', repos = 'https://r-lidar.r-universe.dev')
```

## Example

Here is a simple example of how to classify outliers before to produce a Digital Surface Model (DSM) and a Digital Terrain Model (DTM) from a folder containing airborne LiDAR point clouds. For more example see the  [tutorial](https://r-lidar.github.io/lasR/articles/tutorial.html).

```r
library(lasR)
folder = "/folder/of/laz/tiles/"
pipeline = classify_with_ivf() + chm(1) + dtm(1)
exec(pipeline, on = folder, ncores = 16, progress = T)
```

## Main Differences with `lidR`

The following benchmark compares the time and RAM usage of `lasR` and `lidR` for producing a Digital Terrain Model (DTM), a Canopy Height Model (CHM), and a raster containing two metrics derived from elevation (Z) and intensity. The test was conducted on 120 million points stored in 4 LAZ files. For more details, check out the [benchmark vignette](https://r-lidar.github.io/lasR/articles/benchmarks.html).

<img src="man/figures/readme_benchmark.png" style="display: block; margin: auto;" />

- **Pipelines**: `lasR` introduces pipelines to optimally chain multiple operations on a point cloud, a feature not available in `lidR`.
- **Algorithm Efficiency**: `lasR` uses more powerful algorithms designed for speed and efficiency.
- **Language and Performance**: Entirely written in C/C++, `lasR` has no R code except for the API interface. This makes it highly optimized for performance.
- **Memory Usage**: Unlike `lidR`, which loads the point cloud into an R `data.frame`, `lasR` stores point clouds in a C++ structure that is not exposed to the user, minimizing memory usage.
- **Dependencies**: `lasR` has a single strong dependency on `gdal`. If `sf` and `terra` are installed, the user experience is enhanced, but they are not mandatory.
- **Target Audience**: `lidR` serves as a toolbox for data exploration and innovation, making it ideal for research. In contrast, `lasR` is optimized for production, with a focus on speed and memory efficiency, without compromising on these aspects.

For more details, see the relevant [vignette](https://r-lidar.github.io/lasR/articles/why.html#main-differences-between-lasr-and-lidr).

## About

`lasR` is developed openly by [r-lidar](https://www.r-lidar.com/).

The initial development of `lasR` was made possible through the financial support of [Laval University](https://www.ulaval.ca/en). To continue the development of this free software, we now offer consulting, programming, and training services. For more information, please visit [our website](https://www.r-lidar.com/).

## Install dependencies on GNU/Linux

```
sudo add-apt-repository ppa:ubuntugis/ubuntugis-unstable
sudo apt-get update
sudo apt-get install libgdal-dev libgeos-dev libproj-dev
```

## Copyright Information

`lasR` is free and open source and relies on other free and open source tools.

- For `lasR`:
  - © 2023-2024 Jean-Romain Roussel
  - Provided under GPL-3 license.
- For `LASlib` and `LASzip`:
  - © 2007-2021 Martin Isenburg - <http://rapidlasso.com>
  - Provided under LGPL license and modified to be R-compliant by
    Jean-Romain Roussel.
- For `chm_prep`:
  - © 2008-2023 Benoît St-Onge - [Geophoton-inc/chm_prep](https://github.com/Geophoton-inc/chm_prep)
  - Provided under GPL-3 license.
- For `json` parser:
  - Lohmann, N. (2023). JSON for Modern C++ (Version 3.11.3) [Computer software]. https://github.com/nlohmann
  - Provided under MIT license
- For `delaunator`:
  - © 2018 Volodymyr Bilonenko. [delfrrr/delaunator-cpp](https://github.com/delfrrr/delaunator-cpp)
  - Provided under MIT license
- For `Armadillo`:
  - © 2008-2024 Conrad Sanderson (https://conradsanderson.id.au)
  - © 2008-2016 National ICT Australia (NICTA)
  - © 2017-2024 Data61 / CSIRO
  - Provided under Apache license
- For `Cloth Simulation Filter (CSF)`
  - © 2017 State Key Laboratory of Remote Sensing Science, Institute of Remote Sensing Science and Engineering, Beijing Normal University
  - Provided under Apache License
  - W. Zhang, J. Qi, P. Wan, H. Wang, D. Xie, X. Wang, and G. Yan, “An Easy-to-Use Airborne LiDAR Data Filtering Method Based on Cloth Simulation,” Remote Sens., vol. 8, no. 6, p. 501, 2016.
