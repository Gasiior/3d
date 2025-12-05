Wzięłam wierzchołki domku i zrobiłam tablice z indeksami. Wczesniej trojakt miał swoje własne kopie wierzchołków, teraz jest ich mniej.
Zamiast 9 wierzchołków mogę trzymać 7 a reszcze robić przez indeksy dzięki czemu oszczędza się pamięć.
Każdy wierzchołek 6 float'ów (x,y,z,r,g,b) 
jeden float to 4 bajty
jeden index to 2 bajty

4 bajty x 6 floatów = 24 bajty = jeden wierzchołek
przed: 8 wierzchołków x 24 bajty = 192 bajty

po (indeksy): 6 wierzchołków = 6 wierzchołków x 24 bajty = 144 bajty
9 indeksów x 2 bajty = 18 bajtów
144 + 18 = 162 bajty

192 - 162 = 30 bajtów (oszczędność) 
