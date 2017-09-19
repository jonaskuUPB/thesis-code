init_mo_analysis <- function() {
  initWorkSpace()
  x_limits <<- c(-2,0)
  x_label <<- "prediction quality"
  y_limits <<- c(0, 0)
  y_label <<- "Second Objectve"
}

preparePlot <- function(experimentType) {
  if(experimentType=="aggregation") {
    y_limits <<- c(0,200)
    y_label <<- "k-distance"
  } else if (experimentType=="dispersion") {
    y_limits <<- c(-200,0)
    y_label <<- "- k-distance"
  } else if (experimentType=="flocking") {
    y_limits <<- c(-1,0)
    y_label <<- "speed"
    z_limits <<- c(0, 200)
    z_label <<- "k-distance"
  }
}

readDataFromFile <- function(fileName, commentChar = "#", numObj = 2) {
  t <- read.table(fileName, comment.char = commentChar)[,c(1:numObj,(61+numObj):(63+numObj))]
  # print(t)
  if(numObj==2){
    names(t) <- c("Objective1", "Objective2", "ConstraintViolation", "Rank", "CrowdingDistance")
  } else {
    names(t) <- c("Objective1", "Objective2", "Objective3", "ConstraintViolation", "Rank", "CrowdingDistance")
  }
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
  plot_title <- capitalize(paste(infos[1], infos[2], sep = " "))
  plot_sub <- infos[3]
  if(as.logical(infos[4])){ # highlight nondominated samples
    dominated_set <- dominated_solutions(population[, 1:2])
    non_dominated_set <- population[!(population$Objective1 %in% dominated_set$Objective1),]
    plot(dominated_set, xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub, xlab = x_label, ylab = y_label, pch=21, col="black", bg="grey")
    points(non_dominated_set, pch=21, col="black", bg="black")
  }
  else { # just plot
    plot(population[, 1:2], xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub, xlab = x_label, ylab = y_label, pch=21, col="black", bg="grey")
  }
}

analyse_final_pop <- function(directory, infos, numObj = 2) {
  filename <- paste(directory, "final_pop.out", sep = "/")
  final_pop <- readDataFromFile(filename, numObj=numObj)
  infos <- c(infos, c("final population", TRUE))
  plot_objectives(final_pop, infos)
}

analyse_initial_pop <- function(directory, infos, numObj = 2) {
  filename <- paste(directory, "initial_pop.out", sep = "/")
  initial_pop <- readDataFromFile(filename, numObj=numObj)
  infos <- c(infos, c("initial population", FALSE))
  plot_objectives(initial_pop, infos)
}

analyse_all_pop <- function(directory, infos, numObj = 2) {
  filename <- paste(directory, "all_pop.out", sep = "/")
  all_pop <- readDataFromFile(filename, "#", numObj=numObj)
  infos <- c(infos, c("all population"))
  
  pop1 <- all_pop[,1]
  pop2 <- all_pop[,2]
  
  first_obj <- matrix(nrow=52, ncol = 100)
  second_obj <- matrix(nrow=52, ncol = 100)
  
  if(numObj==3) {
    pop3 <- all_pop[,3]
    third_obj <- matrix(nrow=52, ncol = 100)
  }
  
  for (x in 1:100){
    tmp1 <- splitAt(pop1, 53)
    first_obj[,x] <- tmp1[[1]]
    
    tmp2 <- splitAt(pop2, 53)
    second_obj[,x] <- tmp2[[1]]
    
    if(numObj==3) {
      tmp3 <- splitAt(pop3, 53)
      third_obj[,x] <- tmp3[[1]]
    }
    
    if(length(tmp1)>1){
      pop1 <- tmp1[[2]]
      pop2 <- tmp2[[2]]
      if(numObj==3) {
        pop3 <- tmp3[[2]]
      }
    } else {
      pop1 <- c()
      pop2 <- c()
      if(numObj==3) {
        pop3 <- c()
      }
    }
  }
  plot_title <- capitalize(paste(infos[1], strsplit(infos[2], "_")[[1]], sep = " "))
  sub_title <- "Development of first objective"
  boxplot.matrix(first_obj, main =plot_title, sub = sub_title, ylab = x_label, xlab = "generation")
  
  sub_title <- "Development of second objective"
  boxplot.matrix(second_obj, main =plot_title, sub = sub_title, ylab = y_label, xlab = "generation")
  
  if(numObj==3) {
    sub_title <- "Development of third objective"
    boxplot.matrix(third_obj, main =plot_title, sub = sub_title, ylab = z_label, xlab = "generation")
  }
}

load_best_pop <- function(directory, numObj = 2) {
  filename <- paste(directory, "best_pop.out", sep = "/")
  best_pop <- readDataFromFile(filename, numObj=numObj)
}

analyse_experiment <- function(experiment, numObj = 2) {
  info <- strsplit(experiment, "-")[[1]]
  experimentFolder <- paste(experiment, "run_0", sep = "/")
  analyse_initial_pop(experimentFolder, info[2:3], numObj = numObj)
  analyse_final_pop(experimentFolder, info[2:3], numObj = numObj)
  analyse_all_pop(experimentFolder, info[2:3], numObj = numObj)
}

analyse_experiment_series <- function(series) {
  preparePlot(series)
  best_matrix <- list()
  exp <- list()
   for (i in 1:3){
     expName <- paste("mo", series, "Experiment", sep = "-")
     expName <- paste(expName, i, sep = "_")
     exp <- c(exp, paste("Experiment", i, sep="_"))
     # analyse experiment
     analyse_experiment(expName)
     # analyse trajectories
     analyse_trajectories(expName)
     # load best front
     experimentFolder <- paste(expName, "run_0", sep = "/")
     best_matrix[[i]] <- load_best_pop(experimentFolder)[,1:2]
     # load development
   }
  plot_title <- capitalize(series)
  plot_sub <- "Results of the nondominated sets"
  default_par <- par()
  par(mar = c(5,4,4,8), xpd=TRUE)
  colors <- c("red", "blue", "green")
  plot(best_matrix[[1]], xlim = x_limits, ylim = y_limits, main = plot_title, sub = plot_sub, xlab = x_label, ylab = y_label, pch=21, col="black", bg=colors[1])
  points(best_matrix[[2]], pch=21, col="black", bg=colors[2])
  points(best_matrix[[3]], pch=21, col="black", bg=colors[3])
  legend("right", inset=c(-0.22,0), legend = exp, cex=0.8,fill=colors)
  par(default_par)
}

full_mo_analysis <- function() {
  init_mo_analysis()
  analyse_experiment_series("aggregation")
  analyse_experiment_series("dispersion")
  preparePlot("flocking")
  expName = "mo-flocking-Experiment_0"
  analyse_experiment(expName)
  analyse_trajectories(expName)
  expName = "mo-flocking2o-Experiment_1"
  analyse_experiment(expName)
  analyse_trajectories(expName)
  expName = "mo-flocking3o-Experiment_1"
  analyse_experiment(expName, numObj=3)
  analyse_trajectories(expName)
}