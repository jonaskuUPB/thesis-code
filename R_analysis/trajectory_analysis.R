init <- function() {
  setwd("/home/jonas/Documents/Master/thesis")
}

analyse_trajectory <- function(filename) {
  init()
  t <- read.table(filename)
  transparent_grey <- rgb(125,125,125, max=255, alpha=16)
  plot(t, col=transparent_grey, pch=1, cex=0.1, xlab="", ylab="", xaxs="i", yaxs="i", xlim=c(0,1280), ylim=c(720,0))
}

analyse_trajectories <- function(expName) {
  filename_base <- paste(expName, "run_0", "trajectory", "trajectories_generation_0_genome", sep="/")
  for(i in 0:51){
    filename <- paste(filename_base,i, sep="_")
    print(filename)
    if(file.exists(filename)) {
      analyse_trajectory(filename)
    }
  }
}
