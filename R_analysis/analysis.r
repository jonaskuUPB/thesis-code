init <- function() {
  setwd("/home/jonas/Documents/Master/thesis-code/results")
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
  for (i in 1:genSize-1){
    result[i] <- agglomeration(as.numeric(t[i,]))
  }
  if(!is.null(preProcessing))
    result = preProcessing(result)
  # print(result)
  return(result)
}

ks_test <- function(evolutionSet, controlSet){
  ks_result = ks.test(evolutionSet, controlSet, exact = TRUE)
  # print(ks_result$p.value)
  return(ks_result$p.value)
}

cvm_test <- function(evolutionSet, controlSet) {
  cvm <- cramer::cramer.test(evolutionSet, controlSet)
  p <- cvm$p.value
  
  #cvm <- CvM2SL2Test::cvmts.test(evolutionSet, controlSet)
  #p <- CvM2SL2Test::cvmts.pval(cvm, 50, 50) 

  return(p)
}

run_analysis <- function(expTime = "2017_06_07-21_54_17", statsName = "action_gen", testFunc = ks_test, maxGen = 100, run = "run_0") {
  init()
  vec <- vector(,maxGen)
  for(genCount in 1:maxGen-1){
    print(genCount)
    evolution <- loadDataFromFile(fileName = buildFileName(expName = paste("evolution", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = mean)
    random <- loadDataFromFile(fileName = buildFileName(expName = paste("control", expTime, sep = "-"), runId = run,  genNumber = genCount, statsName = statsName), agglomeration = mean)
    vec[genCount] <- testFunc(evolution, random)
  }
  plot(vec, main = paste(statsName, expTime, run), sub=deparse(substitute(testFunc)), xlab = "generation", ylab = "p value")
}

full_analysis <- function() {
  exp <- c("2017_06_01-10_47_18", "2017_06_07-21_54_17", "2017_06_24-10_08_21", "2017_06_28-08_41_49")
  runs <- c(0,0,0,1)
  for(i in 1:length(exp)){
    for(j in 0:runs[i]) {
      run_analysis(expTime = exp[i], run = paste("run", j, sep = "_"))
    }
  }
}
