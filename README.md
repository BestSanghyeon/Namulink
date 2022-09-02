# Namulink
나무위키 경로 탐색기

나무위키의 한 문서에서 다른 임의의 문서까지 링크 이동을 통하여 도착할 수 있는 최단 경로를 계산합니다.

나무위키 데이터베이스 덤프 다운로드 후 파일명을 `namuwili.json`으로 변경하고, `parsing_namuwiki.cpp` -> `id_mapping.cpp` -> `Namulink.cpp` 순으로 컴파일, 실행

`libicon` 라이브러리 설치 필요 (한글 인코딩 관련)
