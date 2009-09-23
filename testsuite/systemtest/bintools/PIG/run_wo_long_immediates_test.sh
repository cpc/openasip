../../../../tce/src/bintools/PIG/generatebits -f ascii -b data/mach.for.man.sched.conn.opt.bem -p data/1kr4ditfft.tce.tpef data/mach.for.man.sched.conn.opt.adf 
echo ascii image
cat 1kr4ditfft.tce.img
rm -f 1kr4ditfft.tce.img

../../../../tce/src/bintools/PIG/generatebits -f mif -b data/mach.for.man.sched.conn.opt.bem -p data/1kr4ditfft.tce.tpef data/mach.for.man.sched.conn.opt.adf
echo mif image
cat 1kr4ditfft.tce.mif
rm -f 1kr4ditfft.tce.mif

../../../../tce/src/bintools/PIG/generatebits -f array -b data/mach.for.man.sched.conn.opt.bem -p data/1kr4ditfft.tce.tpef data/mach.for.man.sched.conn.opt.adf
echo array image
cat 1kr4ditfft.tce.img

