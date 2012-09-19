@echo off
rem Copyright Beman Dawes 2012
rem Distributed under the Boost Software License, Version 1.0.

rem ' dir="ltr"' messes up toc generation
chg interop-proposal.html " dir=\qltr\q" ""

toc -x interop-proposal.html toc.html
echo Fresh table-of-contents generated in toc.html
echo Hand edit into proposal.

toc.html

