#!/bin/bash

# Script de mesure du temps de boot
# À exécuter après redémarrage

echo "🕐 Mesure du temps de boot..."
echo ""

# Temps total
echo "📊 Temps total de boot :"
systemd-analyze time
echo ""

# Services les plus lents
echo "🐌 Services les plus lents :"
systemd-analyze blame --no-pager | head -15
echo ""

# Détail des services critiques
echo "🔍 Services critiques :"
echo "NetworkManager-wait-online.service : $(systemd-analyze blame --no-pager | grep NetworkManager-wait-online | awk '{print $1}')"
echo "wayvnc.service : $(systemd-analyze blame --no-pager | grep wayvnc.service | awk '{print $1}')"
echo "fstrim.service : $(systemd-analyze blame --no-pager | grep fstrim.service | awk '{print $1}')"
echo ""

# Services désactivés
echo "❌ Services désactivés :"
systemctl list-unit-files --state=disabled | grep -E "(bluetooth|hciuart|triggerhappy|cron)"
echo ""

# Espace disque
echo "💾 Espace disque :"
df -h /
echo "" 