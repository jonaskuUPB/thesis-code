init_effectivity_analysis <- function() {
  initWorkSpace()
}

normalize <- function(data) {
  # substracts the mean from the data set
  m = median(data)
  return(data-m)
}

buildFileName <- function(expName = "2017_06_01-10_47_18", runId = "run_0", genNumber = 0, statsName = "action_gen") {
  file <- paste(expName, runId, sep = "/")
  file <- paste(file, "stats", sep = "/")
  file <- paste(file, statsName, sep ="/")
  file <- paste(file, genNumber, sep = "_")
  return(file)
}

loadDataFromFile <- function(fileName, agglomeration = mean, preProcessing = NULL, genSize = 50) {
  result <- vector(,genSize)
  #special treatment for cluster files
  if(grepl("cluster", fileName, fixed = TRUE)){
    con = file(fileName, "r")
    for (i in 0:(genSize-1)){
      line = readLines(con, n=1)
      values <- strsplit(line, " ")
      result[i] <- agglomeration(as.numeric(values[[1]]))
    }
    close(con)
  } else {
    t <- read.table(fileName)
    for (i in 0:(genSize-1)){
      result[i] <- agglomeration(as.numeric(t[i,]))
    }
  }
  if(!is.null(preProcessing))
    result = preProcessing(result)
  return(result)
}

ks_test <- function(evolutionSet, controlSet){
  ks_result = ks.test(evolutionSet, controlSet, exact = TRUE)
  return(ks_result$p.value)
}

cvm_test <- function(evolutionSet, controlSet) {
  cvm <- cramer::cramer.test(evolutionSet, controlSet)
  p <- cvm$p.value
  return(p)
}

run_analysis <- function(expTime = "2017_06_07-21_54_17", statsName = "action_gen", maxGen = 100, run = "run_0", agglo = median) {
  ks_vec <- vector(,maxGen)
  cvm_vec <- vector(,maxGen)
  for(genCount in 0:(maxGen-1)){
    print(genCount)
    evolution <- loadDataFromFile(fileName = buildFileName(expName = paste("evolution", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = agglo)
    random <- loadDataFromFile(fileName = buildFileName(expName = paste("control", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = agglo)
    ks_vec[genCount] <- ks_test(evolution, random)
    cvm_vec[genCount] <- cvm_test(evolution, random)
  }
  tmp <- strsplit(statsName, "_")[[1]]
  statsDisplay <- paste(tmp[1:length(tmp)-1], sep=" ", collapse=" ")
  expDisplay <- paste(strsplit(expTime, "_")[[1]], sep=" ", collapse=" ")
  # runDisplay <- paste(strsplit(run, "_")[[1]], sep=" ",collapse = " ")
  plot_title <- paste(as.character(substitute(agglo)), statsDisplay, "-", expDisplay)
  plot_title <- capitalize(plot_title)
  plot(ks_vec, main = plot_title, sub="KS-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  plot(cvm_vec, main = plot_title, sub="CvM-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  result <- matrix(ncol=2, nrow=maxGen)
  result[,1] = ks_vec
  result[,2] = cvm_vec
  return(result)
}

full_analysis <- function() {
  init_effectivity_analysis()
  #exp <- c("2017_06_01-10_47_18", "2017_06_07-21_54_17", "2017_06_24-10_08_21", "2017_06_28-08_41_49", "2017_07_11-10_07_33", "2017_07_12-10_33_27")
  exp <- c("Experiment_1", "Experiment_2", "Experiment_3", "Experiment_4", "Experiment_5", "Experiment_6")
  runs <- c(0,0,0,0,0,0)
  action_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  speed_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  k_distance_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  action_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  speed_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  k_distance_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  
  max_cluster_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  mean_cluster_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  max_cluster_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  mean_cluster_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  for(i in 1:length(exp)){
    for(j in 0:runs[i]) {
      action_result <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"))
      action_matrix_ks[,i] <- action_result[,1]
      action_matrix_cvm[,i] <- action_result[,2]
      speed_result <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "speed_gen")
      speed_matrix_ks[,i] <- speed_result[,1]
      speed_matrix_cvm[,i] <- speed_result[,2]
      if(i>=4) {
        distance_result <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "k_distance_gen")
        k_distance_matrix_ks[,i] <- distance_result[,1]
        k_distance_matrix_cvm[,i] <- distance_result[,2]
        max_cluster_result <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "cluster_size_gen", agglo=max)
        max_cluster_matrix_ks[,i] <- max_cluster_result[,1]
        max_cluster_matrix_cvm[,i] <- max_cluster_result[,2]
        mean_cluster_result <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "cluster_size_gen", agglo=mean)
        mean_cluster_matrix_ks[,i] <- mean_cluster_result[,1]
        mean_cluster_matrix_cvm[,i] <- mean_cluster_result[,2]
      }
    }
  }
  default_par <- par()
  par(mar = c(5,4,4,8), xpd=TRUE)
  matplot(action_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "action values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(speed_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "speed values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(k_distance_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "k distance values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(max_cluster_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "max cluster sizes", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(mean_cluster_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "mean cluster sizes", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  
  matplot(action_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "action values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(speed_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "speed values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(k_distance_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "k distance values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(max_cluster_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "max cluster sizes", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(mean_cluster_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "mean cluster sizes", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  par(default_par)
}
