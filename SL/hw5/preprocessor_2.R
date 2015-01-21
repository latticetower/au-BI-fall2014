#this preprocessor should 
rm(list = ls(all.names = TRUE))
library(e1071)
library(MASS)
library(raster)
source("PCA/PCAfncs.R")
source("mnist.r") # http://yann.lecun.com/exdb/mnist/

library(data.table)


train <- load_image_file2('mnist/train-data.changed')
train$y <- load_label_file('mnist/train-labels.changed')

test <- load_image_file2('mnist/test-data.changed')
test$y <- load_label_file('mnist/test-labels.changed')

mnist.train <- train$x #[1:10, ]

mnist.test <- test$x #[1:1000, ]
maxheight<-20
show_digit <- function(arr784, col = gray(12:1/12), ...) {
  image(matrix(arr784, nrow = maxheight)[, maxheight:1], col = col, ...)
}
show_digit(as.matrix(mnist.train[1, ]), main = mnist.train[1, 1])

check.connected <- function(digit) {
  #following helper method simply fills connected area with 0 value, from point [r, c]
  newdigit <<- matrix(digit, nrow = maxheight)
  colorer <- function(r, c, lvl = 0) {
    if (newdigit[r, c] == 0) {
        newdigit[r, c] <<- 1
        rp1 <- if (r == nrow(newdigit)) 1 else r + 1
        rm1 <- if (r == 1) nrow(newdigit) else r - 1
        cp1 <- if (c == ncol(newdigit)) 1 else c + 1
        cm1 <- if (c == 1) ncol(newdigit) else c - 1
        colorer(rm1, c, lvl + 1)
        colorer(rp1, c, lvl + 1)
        colorer(r, cm1, lvl + 1)
        colorer(r, cp1, lvl + 1)
        colorer(rp1, cp1, lvl + 1)
        colorer(rp1, cm1, lvl + 1)
        colorer(rm1, cp1, lvl + 1)
        colorer(rm1, cm1, lvl + 1)
    }
  }

  ncomp <- 0
  for (i in 1: nrow(newdigit)) {
    for (j in 1: ncol(newdigit)) {
      if (newdigit[i, j] == 0) {
        ncomp <- ncomp + 1
        colorer(i, j)
      }
    }
  }
  ncomp
}

connected_comp.train <- NULL
for (i in 1: nrow(mnist.train)) {
  connected_comp.train <- c(connected_comp.train, check.connected(mnist.train[i, ]))
}

connected_comp.test <- NULL
for (i in 1: nrow(mnist.test)) {
  connected_comp.test <- c(connected_comp.test, check.connected(mnist.test[i, ]))
}


save_label_file('mnist/train-data.connected.comp', nrow(mnist.train), connected_comp.train)

save_label_file('mnist/test-data.connected.comp', nrow(mnist.test), connected_comp.test)



