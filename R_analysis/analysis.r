init <- function() {
  setwd("/home/jonas/Documents/Master/thesis-code/results")
}

loadGeneration <- function(expName = "2017_06_01-10_47_18", expId = 0, genNumber = 0, genSize = 50, agglo = mean){
  file <- paste(expName, expId, sep = "/")
  file <- paste(file, "actions", sep = "/")
  file <- paste(file, "action_gen", sep ="/")
  file <- paste(file, genNumber, sep = "_")
  t <- read.table(file)
  result <- vector(,genSize)
  for (i in 1:genSize-1){
    result[i] <- agglo(as.numeric(t[i,]))
  }
  # print(result)
  return(result)
}

loadGenerationBehaviourEstimate <- function(expName = "2017_06_01-10_47_18", expId = 0, genNumber = 0, genSize = 50, agglo = mean){
  
}

ks_test <- function(expName = "2017_06_01-10_47_18"){
  init()
  vec <- vector(,100)
  for(genCount in 1:99){
    print(genCount)
    evolution <- loadGeneration(expName = expName, expId = 0, agglo = median, genNumber = genCount)
    random <- loadGeneration(expName = expName, expId = 1, agglo = median, genNumber = genCount)
    ks_result = ks.test(evolution, random, exact = TRUE)
    # print(ks_result$p.value)
    vec[genCount] <- ks_result$p.value
  }
  plot(vec)
}

kvm_test <- function(expName = "2017_06_01-10_47_18") {
  init()
  vec <- vector(,100)
  for(genCount in 1:99){
    print(genCount)
    evolution <- loadGeneration(expName = expName, expId = 0, agglo = median, genNumber = genCount)
    random <- loadGeneration(expName = expName, expId = 1, agglo = median, genNumber = genCount)
    cvm <- CvM2SL2Test::cvmts.test(evolution, random)
    p <- CvM2SL2Test::cvmts.pval(cvm, 50, 50) 
    # print(ks_result$p.value)
    vec[genCount] <- p
  }
  plot(vec)
}
