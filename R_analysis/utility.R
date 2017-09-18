initWorkSpace <- function() {
  setwd("/home/jonas/Documents/Master/thesis")
}

capitalize <- function(x) {
  s <- strsplit(x, " ")[[1]]
  paste(toupper(substring(s, 1, 1)), substring(s,2), sep="", collapse = " ")
}

splitAt <- function(x, pos) {
  unname(split(x, cumsum(seq_along(x) %in% pos)))
}

analyse_everything <- function() {
  full_analysis()
  full_mo_analysis()
}
