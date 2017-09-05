init <- function() {
  setwd("/home/jonas/Documents/Master/thesis")
  x_limits <<- c(-2,0)
  x_label <<- "prediction quality"
  y_limits <<- c(0, 0)
  y_label <<- "Second Objectve"
}

preparePlot <- function(experimentType) {
  if(experimentType=="aggregation") {
    y_limits <<- c(0,200)
    y_label <<- "k-distance"
  }
}

readDataFromFile <- function(fileName) {
  t <- read.table(fileName)[,c(1:2,63:65)]
  names(t) <- c("Objective1", "Objective2", "ConstraintViolation", "Rank", "CrowdingDistance")
  # t <- t[, c(2,1,3,4,5)]
  return(t)
}

dominated_solutions <- function(pop) {
  D <- pop[order(pop$Objective1, pop$Objective2), ]
  dominated <- D[which(duplicated(cummin(D$Objective2))), ]
  return(dominated)
}

#infos[1] = Type
#infos[2] = Experiment Name
#infos[3] = Population
#infos[4] = Highlight Nondominating samples
plot_objectives <- function(population, infos) {
  # par(pty="s")
  # print(infos)
  plot_title <- paste(infos[1], infos[2], sep = " ")
  plot_sub <- infos[3]
  if(as.logical(infos[4])){ # highlight nondominated samples
    dominated_set <- dominated_solutions(population[, 1:2])
    non_dominated_set <- population[!(population$Objective1 %in% dominated_set$Objective1),]
    print(non_dominated_set)
    #plot(population[, 1:2], xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub)
    plot(dominated_set, xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub, pch=21, col="black", bg="grey")
    points(non_dominated_set, pch=21, col="black", bg="black")
  }
  else { # just plot
    plot(population[, 1:2], xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub, xlab = x_label, ylab = y_label, pch=21, col="black", bg="grey")
  }
}

analyse_final_pop <- function(directory, infos) {
  filename <- paste(directory, "final_pop.out", sep = "/")
  final_pop <- readDataFromFile(filename)
  infos <- c(infos, c("final population", TRUE))
  plot_objectives(final_pop, infos)
}

analyse_initial_pop <- function(directory, infos) {
  filename <- paste(directory, "initial_pop.out", sep = "/")
  initial_pop <- readDataFromFile(filename)
  infos <- c(infos, c("initial population", FALSE))
  plot_objectives(initial_pop, infos)
}

analyse_experiment <- function(experiment) {
  info <- strsplit(experiment, "-")[[1]]
  preparePlot(info[2])
  experimentFolder <- paste(experiment, "run_0", sep = "/")
  analyse_initial_pop(experimentFolder, info[2:3])
  analyse_final_pop(experimentFolder, info[2:3])
}

full_mo_analysis <- function() {
  init()
  analyse_experiment("mo-aggregation-Experiment_1")
}