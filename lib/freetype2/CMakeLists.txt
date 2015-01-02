project(freetype2)

include_directories(include)

set(SRC_DIR src)

add_definitions("-DFT2_BUILD_LIBRARY")

set(SRC 
	${SRC_DIR}/base/ftadvanc.c
	${SRC_DIR}/base/ftbbox.c
	${SRC_DIR}/base/ftbitmap.c
	${SRC_DIR}/base/ftcalc.c
	${SRC_DIR}/base/ftcid.c
	${SRC_DIR}/base/ftdbgmem.c
	${SRC_DIR}/base/ftdebug.c
	${SRC_DIR}/base/ftfstype.c
	${SRC_DIR}/base/ftgasp.c
	${SRC_DIR}/base/ftgloadr.c
	${SRC_DIR}/base/ftglyph.c
	${SRC_DIR}/base/ftgxval.c
	${SRC_DIR}/base/ftinit.c
	${SRC_DIR}/base/ftlcdfil.c
	${SRC_DIR}/base/ftmm.c
	${SRC_DIR}/base/ftobjs.c
	${SRC_DIR}/base/ftotval.c
	${SRC_DIR}/base/ftoutln.c
	${SRC_DIR}/base/ftpatent.c
	${SRC_DIR}/base/ftpfr.c
	${SRC_DIR}/base/ftrfork.c
	${SRC_DIR}/base/ftsnames.c
	${SRC_DIR}/base/ftstream.c
	${SRC_DIR}/base/ftstroke.c
	${SRC_DIR}/base/ftsynth.c
	${SRC_DIR}/base/ftsystem.c
	${SRC_DIR}/base/fttrigon.c
	${SRC_DIR}/base/fttype1.c
	${SRC_DIR}/base/ftutil.c
	${SRC_DIR}/base/ftwinfnt.c
	${SRC_DIR}/base/ftxf86.c
	${SRC_DIR}/autofit/autofit.c
	${SRC_DIR}/bdf/bdf.c
	${SRC_DIR}/bzip2/ftbzip2.c
	${SRC_DIR}/cache/ftcache.c
	${SRC_DIR}/cff/cff.c
	${SRC_DIR}/cid/type1cid.c
	${SRC_DIR}/gzip/ftgzip.c
	${SRC_DIR}/lzw/ftlzw.c
	${SRC_DIR}/pcf/pcf.c
	${SRC_DIR}/pfr/pfr.c
	${SRC_DIR}/psaux/psaux.c
	${SRC_DIR}/pshinter/pshinter.c
	${SRC_DIR}/psnames/psmodule.c
	${SRC_DIR}/raster/raster.c
	${SRC_DIR}/sfnt/sfnt.c
	${SRC_DIR}/smooth/smooth.c
	${SRC_DIR}/truetype/truetype.c
	${SRC_DIR}/type1/type1.c
	${SRC_DIR}/type42/type42.c
	${SRC_DIR}/winfonts/winfnt.c
)

add_library(freetype2 ${SRC})

set(LIBRARY_OUTPUT_PATH ${freetype2_SOURCE_DIR}/bin)
