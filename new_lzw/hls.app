<project xmlns="com.autoesl.autopilot.project" name="new_lzw" top="lzw_encode_hw">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../../Server/lzw_testbench.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="Server/lzw.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="Server/lzw.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="Server/lzw_hw.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="Server/lzw_hw.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

