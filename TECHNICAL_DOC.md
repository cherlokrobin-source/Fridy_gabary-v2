# 📄 Document Technique : Gabary V2 & Djomoa Engine

## 1. Description du Système
**Gabary V2** est un système embarqué de calcul chronologique et de visualisation temps réel via une interface TUI (Terminal User Interface). Il repose sur le moteur **Djomoa Solar Engine V2**, capable d'effectuer des conversions et des projections temporelles absolues sur un horizon de **50 000 ans** sans dérive algorithmique.

---

## 2. Spécifications Architecturelles & Performance

* **Langage de Développement :** C++17 (Norme ISO)
* **Framework Graphique :** FTXUI (Functional Terminal User Interface)
* **Complexité Temporelle :** $\mathcal{O}(1)$ (Temps Constant pour toute requête ID)
* **Empreinte Mémoire (RAM) :** $< 1.0 \text{ MB}$ (Exécution sur Stack sans allocation dynamique lourde)
* **Système de Build :** CMake $\ge 3.14$
* **Environnement Cible :** Termux / Linux ARM64 & x86_64

---

## 3. Structure Modulaire du Code Source

```text
Gabary_V2_UI/
├── SolarEngineV2.h / .cpp    : Cœur logique et calculs astronomiques (O(1))
├── GlobalSolarDay.h / .cpp   : Modèle de données représentatif d'une journée
├── GabaryUI.h / .cpp         : Rendu des composants graphiques (Grille, Gauges)
└── main.cpp                  : Boucle d'événements et gestion des entrées utilisateur

cat << 'EOF' >> TECHNICAL_DOC.md

### Avantages de la Modularité :
1. **Séparation des Responsabilités (SoC) :** Le moteur logique est totalement indépendant du rendu visuel.
2. **Temps de Compilation Optimisé :** Les modifications apportées à l'UI ne déclenchent pas la recompilation du moteur logique.
3. **Traçabilité Git :** Historique de commits propre et ciblé par module.

---

## 4. Algorithmes Fondamentaux

### 4.1. Mappage Indexé d'ID Absolu
Pour éviter les boucles itératives séquentielles ($\mathcal{O}(N)$), chaque jour est indexé par un identifiant entier 64-bit (`Absolute ID`). La résolution du cycle de 400 ans ($146\,097$ jours) s'effectue via :

$$\text{Cycle}_{400} = \left\lfloor \frac{\text{ID} - 1}{146097} \right\rfloor + 1$$

### 4.2. Alignement de la Matrice Mensuelle
La grille académique calcule l'offset du premier jour du mois par rapport à la semaine de 7 jours (du Vendredi au Jeudi) :

$$\text{Start Offset} = (\text{First Day ID} - 1) \pmod 7$$

---

## 5. Procédure de Compilation et Déploiement

```bash
# Compilation du projet
cd build
cmake ..
make -j$(nproc)

# Exécution de l'application
./gabary_tui

Auteur et Affiliation
​Développeur Principal : Nemimeche Benaissa
​Établissement : Université Djillali Liabès (UDL), Sidi Bel Abbès
​Projet : Gabary V2 / Djomoa Engine
