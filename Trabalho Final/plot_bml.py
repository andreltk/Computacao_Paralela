#%matplotlib notebook
import subprocess
from matplotlib.typing import RGBColorType
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from IPython.display import HTML
import os
import time

# Parâmetros que podem ser definidos pelo usuário
N = 512          # Dimensão da grid
STEPS = 2000     # Quantidade de passos
DENSITY_R = 0.2 # Densidade inicial de carros vermelhos
DENSITY_B = 0.2 # Densidade inicial de carros azuis

def find_bml_executable():
    """
    Procura pelo executável 'bml' na pasta atual e em suas subpastas.
    Retorna o caminho absoluto se encontrado, caso contrário retorna None.
    """
    for root, dirs, files in os.walk("."):
        if "bml" in files:
            path = os.path.join(root, "bml")
            if os.access(path, os.X_OK):
                return os.path.abspath(path)
    return None

def run_bml_real_time(N, STEPS, DENSITY_R, DENSITY_B):
    """
    Executa o programa 'bml' em C em tempo real, captura a saída passo a passo,
    atualiza o plot e calcula o tempo entre os passos e passos por segundo.
    """
    executable = find_bml_executable()
    if executable is None:
        raise FileNotFoundError("Não foi possível encontrar o executável 'bml' na pasta atual ou em subpastas.")

    # Inicia o subprocesso com os argumentos N, STEPS, DENSITY_R, DENSITY_B
    proc = subprocess.Popen([executable, str(N), str(STEPS), str(DENSITY_R), str(DENSITY_B)], 
                            stdout=subprocess.PIPE, text=True)

    cmap = ListedColormap(["#f5f5f5", "#900020", "#054f77"])
    fig, ax = plt.subplots()
    im = None
    ax.set_title("BML Simulation")
    colorbar = None

    steps_data = []
    timestamps = []
    current_grid = []
    reading_grid = False
    step_count = 0
    prev_time = None
    
    # Armazena a duração de cada passo
    step_durations = []
    
    # Inicializa o plot com dados vazios
    im = ax.imshow(np.zeros((N, N)), cmap=cmap, interpolation='nearest')
    colorbar = plt.colorbar(im, ax=ax, ticks=[0,1,2])
    text_box = ax.text(0.02, 0.95, '', transform=ax.transAxes, 
                       verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    while True:
        line = proc.stdout.readline()
        if not line:
            break
        line = line.strip()
        if line.startswith("STEP"):
            # Novo passo detectado
            current_step = int(line.split()[1])
            current_time = time.time()
            if prev_time is not None:
                duration = current_time - prev_time
                step_durations.append(duration)
            prev_time = current_time
            reading_grid = True
            current_grid = []
        else:
            # Linha de dados do grid
            row = list(map(int, line.split()))
            if len(row) == N:
                current_grid.append(row)
                if len(current_grid) == N:
                    # Grid completo lido, atualiza o plot
                    arr = np.array(current_grid)
                    steps_data.append(arr)
                    step_count += 1

                    im.set_data(arr)
                    
                    # Calcula a duração e passos por segundo
                    if step_count > 1:
                        last_duration = step_durations[-1]
                        total_time = prev_time - timestamps[0] if timestamps else 0
                        avg_steps_per_sec = step_count / total_time if total_time > 0 else 0
                    else:
                        last_duration = None
                        avg_steps_per_sec = None
                    
                    # Atualiza o título e a caixa de texto com informações
                    ax.set_title(f"BML Simulation - Step {current_step}")
                    if step_count > 1:
                        text = f"Tempo entre steps: {last_duration:.3f} s\n"
                        if avg_steps_per_sec:
                            text += f"Steps por segundo: {avg_steps_per_sec:.2f}"
                        else:
                            text += "Steps por segundo: N/A"
                        text_box.set_text(text)
                    else:
                        text_box.set_text("")

                    fig.canvas.draw()
                    fig.canvas.flush_events()
                    plt.pause(0.01)  # Pequena pausa para atualizar o plot

                    timestamps.append(prev_time)
    
    proc.wait()
    
    # Após a simulação, calcula estatísticas finais
    if timestamps:
        total_time = time.time() - timestamps[0]
        avg_steps_per_sec = step_count / total_time if total_time > 0 else 0
    else:
        total_time = 0
        avg_steps_per_sec = 0

    print(f"Simulação concluída: {step_count} steps em {total_time:.2f} segundos ({avg_steps_per_sec:.2f} steps/s).")

    # Mantém o plot aberto no Jupyter Notebook
    plt.show()

# Executa a simulação
run_bml_real_time(N, STEPS, DENSITY_R, DENSITY_B)
