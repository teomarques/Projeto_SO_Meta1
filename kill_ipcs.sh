#!/bin/bash

ME=$(whoami)
FIFO="/tmp/VALIDATOR_INPUT"  # Adicione esta linha

# Remover pipe nomeado se existir
if [ -p "$FIFO" ]; then
  rm -f "$FIFO"
  echo "Removed named pipe: $FIFO"
fi

# Sua lógica original de IPC
IPCS_S=$(ipcs -s | awk -v user="$ME" '$3 == user {print $2}')
IPCS_M=$(ipcs -m | awk -v user="$ME" '$3 == user {print $2}')
IPCS_Q=$(ipcs -q | awk -v user="$ME" '$3 == user {print $2}')

# Remoção segura
for id in $IPCS_M; do ipcrm -m $id; done
for id in $IPCS_S; do ipcrm -s $id; done
for id in $IPCS_Q; do ipcrm -q $id; done

echo "Cleanup completo para usuário $ME"