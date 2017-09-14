init_trajectory_analysis <- function() {
  initWorkSpace()
}

analyse_trajectory <- function(filename) {
  t <- read.table(filename)
  s <- strsplit(filename, "/")[[1]]
  exp <- strsplit(s[1], "-")[[1]][2:3]
  # print(exp)
  gen <- strsplit(s[4], "_")[[1]][5]
  plot_title <- paste("Trajectory", capitalize(exp[1]), capitalize(exp[2]), "Genome", capitalize(gen), sep=" ")
  sub_title <- paste("Genome", capitalize(gen), "of the final non-dominated set")
  transparent_grey <- rgb(125,125,125, max=255, alpha=16)
  plot(t, col=transparent_grey, pch=1, cex=0.1, xlab="", ylab="", xaxs="i", yaxs="i", xlim=c(0,1280), ylim=c(720,0), main=plot_title, sub=sub_title)
}

analyse_trajectories <- function(expName) {
  init_trajectory_analysis()
  filename_base <- paste(expName, "run_0", "trajectory", "trajectories_generation_0_genome", sep="/")
  for(i in 0:51){
    filename <- paste(filename_base,i, sep="_")
    print(filename)
    if(file.exists(filename)) {
      analyse_trajectory(filename)
    }
  }
}
