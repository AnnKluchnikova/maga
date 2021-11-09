import os
import csv

# Получение аргументов из командной строки
from sys import argv
from argparse import ArgumentParser

def main():
# 
  def keys_parser():
  # 
    parser = ArgumentParser()
    # Все ключи не обязательные
    parser.add_argument ('-c', '--count', type=int)
    parser.add_argument ('-f', '--file')

    return parser
  # 

  def write_data_to_csv(data):
  # 
    print(f"[DEBUG] PID: {pid}: Action: printing data to csv-file")
    csv_file = open('results.csv', 'w')
    with csv_file:
      writer = csv.writer(csv_file)
      writer.writerows(data)
  # 

  print(f"[DEBUG] PID: {pid}: Action: programm started")

  parser = keys_parser()
  key_space = parser.parse_args(argv[1:])

  data = list()
  data.append(['PModel', 'Task', 'OpType', 'Opt', 'InsCount', 'Timer',
                'Time', 'LNum','AvTime','AbsErr','RelErr','TaskPerf'])

 #  Считываем данные экспериментов из временного файла
  if key_space.file  is not None:
    with open(key_space.file,'r') as fd:
      for line in fd:
        data.append([word for word in line.rstrip('\n').split(';')])
  else:
    print(f"[ERROR]  PID: {pid}: Action: programm stopped: "
               "Reason: no file path")
    exit()

  # Делаем дополнительные рассчеты
     # 1й множитель - количество запусков программы
     # 2й множитель - количество функций (для каждого типа)
     # 3й множитель - количество запусков функций
  full_count = 3 * 3* key_space.count
  clock = 0
  stack_count = 0
  for i in range(full_count + 1):
  # 
    if i is not 0:
    # 
      clock = clock + float(data[i][6]) # Ячейка данных [Time] - №6
      stack_count += 1

      if stack_count is key_space.count:
      # 
        # Подсчет среднего времени для серии экспериментов
        average_time_f = clock / key_space.count
        average_time_s = '{:.6f}'.format(average_time_f)
        # print(f"[DEBUG] PID: {pid}: Action: count average time:"
               # f" Value: {average_time_s}")

        for j in range(i-key_space.count + 1, i + 1):
        # 
         # Подсчет абсолютной погрешности
          absolute_error_f = abs(float(data[j][6]) - average_time_f)
          absolute_error_s = '{:.6f}'.format(absolute_error_f)

          # Подсчет относительной погрешности
          relative_error_s = '{:.1f}'.format((absolute_error_f / average_time_f) * 100)

          # Ячейка данных [InsCount] - №4
          task_performance_s ='{:.2f}'.format(int(data[j][4]) / float(data[j][6]))

          data[j].insert(8, str(average_time_s)) # Ячейка данных [AvTime] - №8
          data[j].insert(9, str(absolute_error_s))  # Ячейка данных [AbsError] - №9
          data[j].insert(10, str(relative_error_s))  # Ячейка данных [RelError] - №10
          data[j].insert(11, str(task_performance_s))  # Ячейка данных [TaskPerf] - №11
        # 

        clock = stack_count = 0
      # 
    # 
  # 

  print(f"[DEBUG] PID: {pid}: Data:")
  # for line in data:
  #   print(line)

  write_data_to_csv(data)
  print(f"[DEBUG] PID: {pid}: Action: programm finished")
# 

# ------------------------------------------------------------------------------
pid = os.getpid()
main()
