@set _BATCH_PATH=%~dp0
@set _BATCH_PATH=%_BATCH_PATH:~0,-1%

@rem we have to use cd ... because when we launch the batch as an administrator, the current path is system...
@cd %_BATCH_PATH%
@rem @if not exist generated (
	@rem @mkdir "generated"
@rem )
@set _BATCH_NAME=%~n0
@set _BATCH_NAME=%_BATCH_NAME:~0,-8%
@rem %_BATCH_NAME%.bat -nopause > generated/stdout%_BATCH_NAME%.txt 2> generated/stderr%_BATCH_NAME%.txt
%_BATCH_NAME%.bat -nopause > stdout%_BATCH_NAME%.txt 2> stderr%_BATCH_NAME%.txt