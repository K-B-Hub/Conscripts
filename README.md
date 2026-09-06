# 로그라이크 SRPG (Conscripts)

턴제 SRPG 전투에 로그라이크를 결합한 게임.

| | |
|---|---|
| 개발 기간 | 2026.03 – 진행중 |
| 엔진 · 언어 | Unreal Engine 5.8 · C++ (블루프린트는 에디터 노출과 에셋 연결에만 사용) |
| 인원 | 2인 — 개발 전 영역 / 에셋 · 맵 디자인 1인 |
| 상태 | 핵심 로직과 전투 시스템 구조 완료, 전체 런 구성 중 |

---

## 1. 스킬 시스템 — 독립적인 축의 조합

하나의 클래스에 플래그를 나열하는 대신, 독립적인 축을 각각 열거형으로 분리하고 축의 조합으로 스킬을 정의.

**액티브** — 7개 축
- 선택 방식 (자기 대상 / 단일 지정 / 지면 지정)
- 지정 가능 진영, 범위 적용 대상, 범위 형태 (직선 / 부채꼴 / 원)
- 스킬 유형, 피해 유형, 밀치기 기준점

**패시브** — 13개 발동 시점
- `Stat` 단순 스탯 / `Conditional` 조건부 9종 / `Reactive` 반응형 4종
- 조건부: 턴 시작, 턴 종료, 라운드 시작, 피격, 치사 피해 직전, 아군 사망, 적 사망, 이동 완료, 지형 변경
- 반응형: 이동 시작 전, 피해 계산 전, 피해 적용 후, 처치 후
- 시점마다 대응하는 가상 함수. 전투 흐름은 해당 시점에 등록된 스킬을 호출만 함
- 새 스킬 = 파생 클래스에서 함수 재정의. 전투 흐름 코드 수정 없음

```
Source/PW/Enum/SkillTypes.h                 축 정의 — 여기부터 보면 구조가 한눈에 들어옴
Source/PW/Object/Skill/ActiveSkillBase.h
Source/PW/Object/Skill/PassiveSkillBase.h   발동 시점별 Execute_ 가상 함수
Source/PW/ActorComponent/PassiveSkillComponent.h
Source/PW/Object/CommonUpgrade/             축 위에 얹은 업그레이드 패시브
```

## 2. Utility AI — 서로 다른 수치를 하나의 단위로 환산

행동 후보(스킬 × 대상 × 시전 위치, 순수 이동, 대기)를 열거해 점수화, 최고점 실행.

피해, 회복, 버프, 상태이상, 지형 손익, 시전 위치의 피격 위험을 모두 **기대 체력**으로 환산해 비교.

- 스탯 1점의 가치 = 대상의 턴당 기대 가해 피해와 피격 피해 기준으로 환산
- 스테이지 내 평균 스탯에서 벗어난 정도를 배율로 반영
- 오버킬 · 오버힐은 점수에 패널티, 아군 오사는 음의 가치
- 관측 기반 위협 — 상대가 실제로 사용한 스킬만 위협 프로파일로 누적
- 성향은 코드 분기가 아니라 데이터 애셋의 가중치로 분리

```
Source/PW/AI/UtilityAIComponent.h / .cpp   후보 열거 · 환산 · 점수 산출
Source/PW/AI/AIAction.h                    행동 후보 구조체
Source/PW/AI/AIPersonalityData.h           성향별 가중치 데이터 애셋
```

## 3. 3D 최적 시전 위치 탐색

스킬 지정 → 사거리와 시야 조건을 만족하는 위치를 찾아 이동 후 시전.

1. 조준 지점 결정 (대상 / 지면점 / 최다 적중 지점)
2. 피보나치 디스크 분포로 후보 위치 균등 샘플링
3. 지형 배율을 반영한 실질 이동 비용으로 도달 판정
4. 사거리 게이트 + 시야선(LOS) 필터
5. 이동 비용 · 통과 지형 · 도착 지점 위험까지 포함해 기대 체력으로 평가

- 도달 판정은 실질 비용, 거리 페널티는 실제 경로 길이 — 두 값을 분리해 위험 지형 우회가 성립
- 탐색 로직은 스킬 조건을 질의만 하고 내용은 모름 → 플레이어 자동 이동과 AI 후보 열거가 같은 코드 공유

```
Source/PW/AI/AINavigationHelper.h / .cpp   도달 판정 · 시야선 · 지형 경로 비용
Source/PW/Actors/AttackRangeIndicator.cpp  사거리 · 범위 표시
```

## 4. 그 외

```
Source/PW/GameMode/BattleGameMode.cpp      전투 진행
Source/PW/PlayerController/BattleController.cpp
Source/PW/Actors/Terrain/                  지형 — 체류 · 통과 손익, 이동 비용 배율
Source/PW/Object/Stress/                   정신력 소모에 따른 버프 / 상태이상
Source/PW/DataAsset/UpgradeLibrary.cpp     4등급 업그레이드 풀
Source/PW/AI/BTTask/                       전투 밖 순찰 · 탐지 (행동 트리)
```
