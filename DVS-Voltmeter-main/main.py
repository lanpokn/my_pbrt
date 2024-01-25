#! python3
# -*- encoding: utf-8 -*-
'''
@File    :   main.py
@Time    :   2022/7/12 17:30
@Author  :   Songnan Lin, Ye Ma
@Contact :   songnan.lin@ntu.edu.sg, my17@tsinghua.org.cn
@Note    :   
@inproceedings{lin2022dvsvoltmeter,
  title={DVS-Voltmeter: Stochastic Process-based Event Simulator for Dynamic Vision Sensors},
  author={Lin, Songnan and Ma, Ye and Guo, Zhenhua and Wen, Bihan},
  booktitle={ECCV},
  year={2022}
}
'''

import argparse
import os
import numpy as np
import cv2
import tqdm
from src.config import cfg
from src.simulator import EventSim
from src.visualize import events_to_voxel_grid, visual_voxel_grid
def get_args_from_command_line():
    parser = argparse.ArgumentParser(description='Parser of Runner of Network')
    parser.add_argument('--camera_type', type=str, help='Camera type, such as DVS346', default='DVS346')
    parser.add_argument('--model_para', type=float, nargs='+', help='Set parameters for a specific camera type', default=None)
    parser.add_argument('--input_dir', type=str, help='Set dataset root_path', default=None)
    parser.add_argument('--output_dir', type=str, help='Set output path', default=None)
    args = parser.parse_args()
    return args

def integrate_cfg(cfg, command_line_args):
    args = command_line_args
    cfg.SENSOR.CAMERA_TYPE = args.camera_type if args.camera_type is not None else cfg.SENSOR.CAMERA_TYPE
    cfg.SENSOR.K = args.model_para if args.model_para is not None else cfg.SENSOR.K
    cfg.DIR.IN_PATH = args.input_dir if args.input_dir is not None else cfg.DIR.IN_PATH
    cfg.DIR.OUT_PATH = args.output_dir if args.output_dir is not None else cfg.DIR.OUT_PATH
    if cfg.SENSOR.K is None or len(cfg.SENSOR.K) != 6:
        raise Exception('No model parameters given for sensor type %s' % cfg.SENSOR.CAMERA_TYPE)
    print(cfg)
    return cfg

def is_valid_dir(dirs):
    return os.path.exists(os.path.join(dirs, 'info.txt'))

##TODO, need a new process_dir,read video as input, average dt
def process_video_frames(cfg,video_path,dt=2857):
    outdir = os.path.join(cfg.DIR.OUT_PATH, os.path.basename(video_path))
    print(f"Processing video {video_path}... Generating events in file {outdir}")


    # set simulator
    sim = EventSim(cfg=cfg, output_folder=cfg.DIR.OUT_PATH, video_name=os.path.basename(video_path))

    # process
    num_events, num_on_events, num_off_events = 0, 0, 0
    events = []
    cap = cv2.VideoCapture(video_path)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT)) -1
    pbar = tqdm.tqdm(total=total_frames)
    for i in range(total_frames):
        timestamp_us = dt*i
        
        # read frame from video
        ret, frame = cap.read()
        # just use gray graph!!!
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        if not ret:
            print(f"Error reading frame {i} from video {video_path}.")
            break

        # event generation!!!
        event = sim.generate_events(frame, timestamp_us)

        if event is not None:
            events.append(event)
            num_events += event.shape[0]
            num_on_events += np.sum(event[:, -1] == 1)
            num_off_events += np.sum(event[:, -1] == 0)

        pbar.set_description(f"Events generated: {num_events}")
        pbar.update(1)

    cap.release()
    events = np.concatenate(events, axis=0)
    name = "Rotate_360_high"
    np.savetxt("D:/2023/computional imaging/my_pbrt/DVS-Voltmeter-main/data_samples/output/" +name+ '.txt', events, fmt='%1.0f')
    sim.reset()


def process_dir(cfg, file_info, video_name):
    indir = os.path.join(cfg.DIR.IN_PATH, video_name)
    outdir = os.path.join(cfg.DIR.OUT_PATH, video_name)
    print(f"Processing folder {indir}... Generating events in file {outdir}")

    # file info
    file_timestamps_us = [int(info_i.split()[1]) for info_i in file_info]
    file_paths = [info_i.split()[0] for info_i in file_info]

    # set simulator
    sim = EventSim(cfg=cfg, output_folder=cfg.DIR.OUT_PATH, video_name=video_name)

    # process
    pbar = tqdm.tqdm(total=len(file_paths))
    num_events, num_on_events, num_off_events = 0, 0, 0
    events = []
    for i in range(0, len(file_paths)):
        timestamp_us = file_timestamps_us[i]
        image = cv2.imread(file_paths[i], cv2.IMREAD_GRAYSCALE)

        # event generation!!!
        event = sim.generate_events(image, timestamp_us)

        if event is not None:
            events.append(event)
            num_events += event.shape[0]
            num_on_events += np.sum(event[:, -1] == 1)
            num_off_events += np.sum(event[:, -1] == 0)

        pbar.set_description(f"Events generated: {num_events}")
        pbar.update(1)

    events = np.concatenate(events, axis=0)
    np.savetxt(os.path.join(cfg.DIR.OUT_PATH, os.path.basename(video_path) + '.txt'), events, fmt='%1.0f')
    sim.reset()


if __name__ == "__main__":
    # args = get_args_from_command_line()
    # cfg = integrate_cfg(cfg, args)
    # video_list = sorted(os.listdir(cfg.DIR.IN_PATH))
    # for video_i in video_list:
    #     video_i_path = os.path.join(cfg.DIR.IN_PATH, video_i)
    #     os.makedirs(os.path.join(cfg.DIR.OUT_PATH, video_i), exist_ok=True)

    #     if is_valid_dir(video_i_path):
    #         # video info
    #         with open(os.path.join(cfg.DIR.IN_PATH, video_i, 'info.txt'), 'r') as f:
    #             video_info = f.readlines()
    #         # simulation
    #         process_dir(cfg=cfg, file_info=video_info, video_name=video_i)

    args = get_args_from_command_line()
    cfg = integrate_cfg(cfg, args)
    # video_list = sorted(os.listdir(cfg.DIR.IN_PATH))
    video_path = "D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/20_eevee.mkv"
    dt = 2857
    process_video_frames(cfg, video_path, dt)
