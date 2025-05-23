f = paste0(system.file(package="lasR"), "/extdata/bcts/")
f = list.files(f, pattern = "(?i)\\.la(s|z)$", full.names = TRUE)

read = function()
{
  load = function(data) { return(data) }
  call = callback(load, expose = "xyzrib", no_las_update = TRUE)
  call
}

test_that("reader_circle perform a queries",
{
  pipeline = reader_las_circles(885100, 629300, 10, filter = keep_first()) + read()
  ans = exec(pipeline, f)
  expect_equal(dim(ans), c(2665L, 6L))
  expect_equal(table(ans$ReturnNumber), c(`1` = 2665L), ignore_attr = T)

  # between two tiles
  pipeline = reader_las_circles(885150, 629400, 10, filter = keep_ground()) + read()
  ans = exec(pipeline, f)
  expect_equal(dim(ans), c(314L, 6L))

  # between two tiles with a buffer
  pipeline = reader_las_circles(885150, 629400, 10) + read()
  ans = exec(pipeline, f, buffer = 5)

  expect_equal(dim(ans), c(14074, 6L))
  expect_equal(sum(ans$Buffer), 7706L)

  # between two tiles with a buffer
  pipeline = reader_las_circles(885150, 629400, 10, filter = keep_first()) + read()
  ans = exec(pipeline, f, buffer = 5)

  expect_equal(dim(ans), c(10263, 6L))
  expect_equal(sum(ans$Buffer), 5691L)

  # between two tiles with a buffer but the centroid is not in a file
  pipeline = reader_las_rectangles(885000L, 629390, 885040, 629410) + read()
  ans = exec(pipeline, f, buffer = 5)
  expect_equal(dim(ans), c(5028L, 6L))
  expect_equal(sum(ans$Buffer), 2415L)

  # no match
  pipeline = reader_las_circles(8850000, 629400, 20) + read()
  expect_error(exec(pipeline, f, buffer = 5), "cannot find")
})

test_that("reader_circle creates raster with minimal bbox",
{
  pipeline = reader_las_circles(c(885150, 885150), c(629300, 629600) , 10, filter = keep_ground()) + rasterize(2, "min")
  ans = exec(pipeline, on = f)

  expect_equal(dim(ans), c(161, 11, 1))
  expect_equal(sum(is.na(ans[])), 1622)
})

test_that("circle buffer is removed #80",
{
  f <- system.file("extdata", "Topography.las", package = "lasR")
  ans <- exec(reader_las_circles(273500, 5274500, 20) + rasterize(2, "z_mean"), on = f, buffer = 20)

  expect_equal(sum(is.na(ans[])), 136L)
})

