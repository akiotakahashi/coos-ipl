%include "define.mac"

%rep	2
	times SectorsPerFAT*BytesPerSector db 0
%endrep
