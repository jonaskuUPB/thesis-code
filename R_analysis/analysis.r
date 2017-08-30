init <- function() {
  setwd("/home/jonas/Documents/Master/thesis")
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
  t <- read.table(fileName)
  result <- vector(,genSize)
  for (i in 0:(genSize-1)){
    result[i] <- agglomeration(as.numeric(t[i,]))
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

run_analysis <- function(expTime = "2017_06_07-21_54_17", statsName = "action_gen", maxGen = 100, run = "run_0") {
  init()
  ks_vec <- vector(,maxGen)
  cvm_vec <- vector(,maxGen)
  for(genCount in 0:(maxGen-1)){
    print(genCount)
    evolution <- loadDataFromFile(fileName = buildFileName(expName = paste("evolution", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = median)
    random <- loadDataFromFile(fileName = buildFileName(expName = paste("control", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = median)
    ks_vec[genCount] <- ks_test(evolution, random)
    cvm_vec[genCount] <- cvm_test(evolution, random)
  }
  plot(ks_vec, main = paste(statsName, expTime, run), sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  plot(cvm_vec, main = paste(statsName, expTime, run), sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  result <- matrix(ncol=2, nrow=maxGen)
  result[,1] = ks_vec
  result[,2] = cvm_vec
  return(result)
}

full_analysis <- function() {
  #exp <- c("2017_06_01-10_47_18", "2017_06_07-21_54_17", "2017_06_24-10_08_21", "2017_06_28-08_41_49", "2017_07_11-10_07_33", "2017_07_12-10_33_27")
  exp <- c("Experiment_1", "Experiment_2", "Experiment_3", "Experiment_4", "Experiment_5", "Experiment_6")
  runs <- c(0,0,0,0,0,0)
  action_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  speed_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  k_distance_matrix_ks <- matrix(ncol = length(exp), nrow = 100)
  action_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  speed_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  k_distance_matrix_cvm <- matrix(ncol = length(exp), nrow = 100)
  for(i in 1:length(exp)){
    for(j in 0:runs[i]) {
      action_matrix_ks[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"))[,1]
      action_matrix_cvm[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"))[,2]
      speed_matrix_ks[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "speed_gen")[,1]
      speed_matrix_cvm[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "speed_gen")[,2]
      if(i>=4) {
        k_distance_matrix_ks[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "k_distance_gen")[,1]
        k_distance_matrix_cvm[,i] <- run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"), statsName = "k_distance_gen")[,2]
      }
    }
  }
  par(mar = c(5,4,4,8), xpd=TRUE)
  matplot(action_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "action values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(speed_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "speed values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(k_distance_matrix_ks, type = "b", pch=1, col=1:length(exp), main = "k distance values", sub="ks-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  
  matplot(action_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "action values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(speed_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "speed values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
  matplot(k_distance_matrix_cvm, type = "b", pch=1, col=1:length(exp), main = "k distance values", sub="cvm-test", xlab = "generation", ylab = "p value", ylim=c(0,1))
  legend("right", inset=c(-0.22,0), exp, col=1:length(exp), cex=0.8,fill=1:length(exp))
}
