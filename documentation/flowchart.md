flowchart TB
    A["Base de données"] --> B("Parcours de la base")
    B -- 1 image png --> C("Edge detection")
    C -- 1 image png --> D("Localisation des croix")
    D -- Coordonées x,y des croix --> E{"Warpping"}
    E --> Aprime("Parcous de la base de données redréssées")
    Aprime -- 1 image png --> G("Binarisation") & Z("Dilatation")
    G -- 1 image png --> F("Edge detection")
    F --> I("Localisation du label")
    F -- 1 image png --> J("Localisation des sizes")
    F -- 1 image png si num page différente de 22 --> N("Extraction des cases")
    Z -- 1 image png --> Y("Binarisation")
    Y -- 1 image png --> H("Localisation du code binaire")
    H -- Coordonnées du code binaire --> X("Conversion du signal 2D en signal 1D")
    X -- Signal 1D --> K("Extraction du num page et num scripteur")
    I -- Coordonnées des labels --> L("Reconnaissance des labels")
    J -- Coordonnées des sizes (si existantes) --> M("Reconnaissance des sizes")
    Aprime -- Image vierge --> N
    Aprime --> O@{ label: "Extraction de l'intérieur des cases" }
    N -- Vecteurs des cases --> R("Extraction des num ligne et num colonne")
    O -- 1 image png --> P["Création du .png"]
    L -- Une chaîne de caractères --> Q["Création du .txt"]
    K -- Deux entiers --> Q
    M -- Une chaîne de caractères --> Q
    R -- Deux entiers --> Q

    style A stroke:#2962FF,color:#2962FF
    style E color:#00C853,stroke:#00C853
    style Aprime stroke:#2962FF,color:#2962FF
    style K color:#00C853,stroke:#00C853
    style L color:#00C853,stroke:#00C853
    style M color:#00C853,stroke:#00C853
    style O color:#00C853,stroke:#00C853
    style P stroke:#D50000,color:#D50000
    style Q stroke:#D50000,color:#D50000
