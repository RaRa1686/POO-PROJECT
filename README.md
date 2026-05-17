# 🏆 Head Football

Un joc de fotbal 2D dezvoltat în C++ cu SFML 3.

---

## 🎮 Descriere

**Head Football** este un joc în care doi jucători (sau un jucător vs Calculator) controlează personaje cu cap mare și încearcă să marcheze goluri în poarta adversarului. Jocul rulează în timp real cu fizică simplă, coliziuni și un timer de 90 de secunde.

### Moduri de joc
- **1 vs 1 local** — doi jucători pe aceeași tastatură
- **vs Calculator (AI)** — jucător vs inteligență artificială

### Controale
| Jucător 1 | Jucător 2 |
|-----------|-----------|
| `A` — stânga | `←` — stânga |
| `D` — dreapta | `→` — dreapta |
| `W` — săritură | `↑` — săritură |

---

## 📋 Cerințe POO Implementate

### Ierarhie de clase
- `GameObject` *(clasă abstractă de bază)*
  - `Ball` — mingea de joc
  - `Player` *(clasă abstractă)*
    - `HumanPlayer` — jucător controlat de tastatură
    - `AIPlayer` — jucător controlat de calculator
- `GameException` *(ierarhie de excepții)*
  - `GoalException` — excepție aruncată la gol
  - `InvalidMoveException` — excepție pentru mișcări invalide

### Concepte OOP
-  **Constructori** — inițializare, supraîncărcat, copiere
-  **operator=** — copiere corectă pentru toate clasele
-  **Destructor virtual** — în toate clasele de bază
-  **operator<<** și **operator>>** — afișare și citire din stream
-  **Operatori supraîncărcați membri** — `Ball::operator+`, `Player::operator+=`, `Player::operator<`, `GameObject::operator==`
-  **Operatori supraîncărcați non-membri** — `operator*(Ball, float)`, `operator<<(ostream, Player)`
-  **Upcast și Downcast** — demonstrat cu `dynamic_cast<AIPlayer*>`
-  **const** — metode și parametri const peste tot
-  **static** — `totalPlayers`, `totalBounces`, `maxSpeed`, `getInstance()`
-  **Excepții** — ierarhie proprie, `throw`, `try/catch`, upcast în catch
-  **Clasă template** — `ScoreBoard<T>` instanțiată cu `ScoreBoard<int>` și `ScoreBoard<float>`
-  **STL containere** — `std::vector` și `std::list`
-  **STL algorithm cu lambda** — `std::sort`, `std::any_of`, `std::max_element`
-  **Design Pattern Singleton** — `Game::getInstance()`
-  **Design Pattern Observer** — `IObserver` / `ISubject`

---

## 🛠️ Compilare

```bash
cmake -S . -B build
cmake --build build
./build/HeadFootball
```

**Dependință:** SFML 3.0+
```bash
brew install sfml  # macOS
```
