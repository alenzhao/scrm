#!/bin/bash

#Compare summary statistics of ms and scrm for TMRCA, number of mutation, and number of recombination

msNsample=(2 3 4 7 10 20)

rep=100

## compare TMRCA
compareTMRCA=compareTMRCA
#rm ${compareTMRCA}
echo -e "compare TMRCA for ${rep} replicates \n\t|\t\t\t|\t\t ms \t\t|\t\t scrm\nNsam\t|\tmean\tstdv\t|\tmean\tstdv\tstd err\t|\tmean\tstdv \tstd err" >${compareTMRCA}
#theta=10

npop=1000000
for nsam in "${msNsample[@]}"
	do
	prefix=${nsam}sample
	out=${prefix}out
	tmrca=${prefix}tmrca
	Trees=${prefix}Trees
	ms ${nsam} ${rep} -T | tail -n +4 | grep -v "//" > ms${out}
	cat ms${out} | grep ";" | sed -e 's/\[.*\]//g' > ms${Trees}
	hybrid-Lambda -gt ms${Trees} -tmrca ms${tmrca}
	scrm ${nsam} ${rep} -T | tail -n +4 | grep -v "//" > scrm${out}
	cat scrm${out} | grep ";" | sed -e 's/\[.*\]//g' > scrm${Trees}
	hybrid-Lambda -gt scrm${Trees} -tmrca scrm${tmrca}
	echo "rm(list=ls());
	source(\"fun_src.r\");
	msdata=read.table(\"ms${tmrca}\")\$V1;
	scrmdata=read.table(\"scrm${tmrca}\")\$V1;
	ee=ee_tmrca(${nsam});
	sdv=sd_tmrca(${nsam});
	cat(paste(${nsam},\"|\",format(ee,digits=4),format(sdv,digits=4),\"|\",
format(mean(msdata),digits=4),format(sd(msdata),digits=4),format(sd(msdata)/sqrt(length(msdata)),digits=4),\"|\",
format(mean(scrmdata),digits=4),format(sd(scrmdata),digits=4),format(sd(scrmdata)/sqrt(length(scrmdata)),digits=4),
sep=\"\t\"),file=\"${compareTMRCA}\",append=TRUE);cat(\"\n\",file=\"${compareTMRCA}\",append=TRUE);" > dummy.r
	R CMD BATCH dummy.r
	rm ms${out} ms${Trees} ms${tmrca} scrm${out} scrm${Trees} scrm${tmrca} 
	done

