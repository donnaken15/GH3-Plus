@echo off
FOR %%Q IN (Open_NoteFX.qbs GuitarEvent_HitNotes.qbs) DO ( echo %%Q & qbc %%Q %%Qcript )
if ERRORLEVEL 1 pause
:#del *.qbscriptcript*