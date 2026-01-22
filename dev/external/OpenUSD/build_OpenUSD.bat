@REM #change current directory to this file
%~d0
cd %~dp0

@rem if no python, then usdview will be disabled
python OpenUSD-release/build_scripts/build_usd.py --no-python _build


@rem --------------------
@rem pip install PySide6
@rem pip install PyOpenGL

@rem need run as admin
@rem mklink "%AppData%\..\Local\Programs\Python\Python314\Scripts\pyside6-uic.exe" "%AppData%\Python\Python314\site-packages\PySide6\uic.exe"

@rem python OpenUSD-release/build_scripts/build_usd.py _build

@pause