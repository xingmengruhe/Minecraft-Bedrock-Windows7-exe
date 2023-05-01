@echo off
cd %~dp0
echo.
echo --------------------------------------------------
echo Importador de skins
echo --------------------------------------------------
echo.
echo Pon tu skin con el nombre "steve" o "alex" (sin comillas)
echo dentro de la carpeta donde esta este archivo.
echo.
echo Si ya has hecho eso, presiona cualquier tecla
echo para continuar.
echo.
echo --------------------------------------------------
echo.
pause
echo.
move /y steve.png mcdata\data\skin_packs\vanilla
move /y alex.png mcdata\data\skin_packs\vanilla
echo.
echo --------------------------------------------------
echo Hecho! Presiona cualquier tecla y abre Minecraft
echo --------------------------------------------------
echo.
pause