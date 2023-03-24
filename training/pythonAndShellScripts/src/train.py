import numpy as np
import warnings
import os
import tensorflow as tf
from tensorflow import keras
from model_utils import save_model
from tqdm.keras import TqdmCallback

warnings.filterwarnings("ignore")
tf.debugging.enable_check_numerics(True)
tf.config.run_functions_eagerly(True)
tf.compat.v1.enable_eager_execution()

# ORIGINAL construct TensorFlow model
# model = keras.Sequential()
# model.add(keras.layers.InputLayer(input_shape=(None, 1)))
# model.add(keras.layers.Dense(8, activation='tanh', kernel_initializer='random_normal', bias_initializer='random_normal'))
# model.add(keras.layers.Conv1D(4, 3, dilation_rate=2, activation='tanh', padding='causal', kernel_initializer='glorot_uniform', bias_initializer='random_normal'))
# model.add(keras.layers.GRU (8, activation="tanh", return_sequences=True, recurrent_activation="sigmoid", use_bias=True, kernel_initializer="glorot_uniform", recurrent_initializer="orthogonal", bias_initializer="random_normal",))
# model.add(keras.layers.Dense(1, kernel_initializer='orthogonal', bias_initializer='random_normal'))

DATA_DIR = "/path/to/mlData/"

TRAIN_SAMPLES_PER_DIRECTORY = 149
TEST_SAMPLES_PER_DIRECTORY = 19
VALIDATION_SAMPLES_PER_DIRECTORY = 19
AUDIO_SAMPLES_PER_FILE = 44100 # one second of f32 .wav data at 44.1kHz.
NUM_DIRECTORIES = 16

x1_train_file_paths = []
x1_test_file_paths = []
x1_validation_file_paths = []

x2_train_file_paths = []
x2_test_file_paths = []
x2_validation_file_paths = []

y_train_file_paths = []
y_test_file_paths = []
y_validation_file_paths = []

def load_npy_file(file_path):
    data = np.load(file_path, allow_pickle=True)
    return data

def get_int_from_wav(string):
    return int(string.rsplit('/', 1)[-1].rsplit('.')[0])

for root, dirs, files in os.walk(DATA_DIR):
    for file in files:
        if file.endswith(".npy"):
            if "groundTruth" in root:
                file_path = os.path.join(root, file)
                if "train" in file:
                    x1_train_file_paths.append(file_path)
                if "test" in file:
                    x1_test_file_paths.append(file_path)
                if "validation" in file:
                    x1_validation_file_paths.append(file_path)
            if "matronPlugin" in root:
                file_path = os.path.join(root, file)
                if "train" in file:
                    x2_train_file_paths.append(file_path)
                if "test" in file:
                    x2_test_file_paths.append(file_path)
                if "validation" in file:
                    x2_validation_file_paths.append(file_path)
            if "moogFilteredResult" in root:
                file_path = os.path.join(root, file)
                if "train" in file:
                    y_train_file_paths.append(file_path)
                if "test" in file:
                    y_test_file_paths.append(file_path)
                if "validation" in file:
                    y_validation_file_paths.append(file_path)


x1_train_items = [load_npy_file(file_path) for file_path in sorted(x1_train_file_paths, key=lambda
    string: get_int_from_wav(string))]
x1_test_items = [load_npy_file(file_path) for file_path in sorted(x1_test_file_paths, key=lambda
    string: get_int_from_wav(string))]
x1_validation_items = [load_npy_file(file_path) for file_path in sorted(x1_validation_file_paths, key=lambda
    string: get_int_from_wav(string))]

x2_train_items = [load_npy_file(file_path) for file_path in sorted(x2_train_file_paths, key=lambda
    string: get_int_from_wav(string))]
x2_test_items = [load_npy_file(file_path) for file_path in sorted(x2_test_file_paths, key=lambda
    string: get_int_from_wav(string))]
x2_validation_items = [load_npy_file(file_path) for file_path in sorted(x2_validation_file_paths, key=lambda
    string: get_int_from_wav(string))]

y_train_items = [load_npy_file(file_path) for file_path in sorted(y_train_file_paths, key=lambda
    string: get_int_from_wav(string))]
y_test_items = [load_npy_file(file_path) for file_path in sorted(y_test_file_paths, key=lambda
    string: get_int_from_wav(string))]
y_validation_items = [load_npy_file(file_path) for file_path in sorted(y_validation_file_paths, key=lambda
    string: get_int_from_wav(string))]

x_train_items = np.asarray(list(zip(x1_train_items, x2_train_items)))
x_test_items = np.asarray(list(zip(x1_test_items, x2_test_items)))
x_validation_items = np.asarray(list(zip(x1_validation_items, x2_validation_items)))

x_train_items_T = (np.transpose(x_train_items, (0, 2, 1)))
x_test_items_T = (np.transpose(x_test_items, (0, 2, 1)))
x_validation_items_T = (np.transpose(x_validation_items, (0, 2, 1)))

x_train_items_T_reshaped = x_train_items_T.reshape(NUM_DIRECTORIES * TRAIN_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 2)
x_test_items_T_reshaped = x_test_items_T.reshape(NUM_DIRECTORIES * TEST_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 2)
x_validation_items_T_reshaped = x_validation_items_T.reshape(NUM_DIRECTORIES * VALIDATION_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 2)

x_train_items_add_batch = np.expand_dims(x_train_items_T_reshaped, axis=0)
x_test_items_add_batch = np.expand_dims(x_test_items_T_reshaped, axis=0)
x_validation_items_add_batch = np.expand_dims(x_validation_items_T_reshaped, axis=0)

y_train_items = np.expand_dims(np.asarray(y_train_items), axis=-1)
y_test_items = np.expand_dims(np.asarray(y_test_items), axis=-1)
y_validation_items = np.expand_dims(np.asarray(y_validation_items), axis=-1)

y_train_items_reshaped = y_train_items.reshape(NUM_DIRECTORIES * TRAIN_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 1)
y_test_items_reshaped = y_test_items.reshape(NUM_DIRECTORIES * TEST_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 1)
y_validation_items_reshaped = y_validation_items.reshape(NUM_DIRECTORIES * VALIDATION_SAMPLES_PER_DIRECTORY * AUDIO_SAMPLES_PER_FILE, 1)

y_train_items_add_batch = np.expand_dims(y_train_items_reshaped, axis=0)
y_test_items_add_batch = np.expand_dims(y_test_items_reshaped, axis=0)
y_validation_items_add_batch = np.expand_dims(y_validation_items_reshaped, axis=0)

# NEW construct TensorFlow model

model = keras.Sequential()
model.add(keras.layers.InputLayer(input_shape=(None, 2)))
model.add(keras.layers.Dense(8, name="dense1", activation='tanh', kernel_initializer='random_normal', bias_initializer='random_normal'))
model.add(keras.layers.Conv1D(4, 3, name="conv1-1", dilation_rate=2, activation='tanh', padding='causal', bias_initializer='random_normal'))
model.add(keras.layers.Conv1D(2, 3, name="conv1-2", dilation_rate=35, activation='tanh', padding='causal', bias_initializer='zeros'))
# model.add(keras.layers.GRU (2, activation="tanh", return_sequences=True, recurrent_activation="sigmoid", use_bias=True, bias_initializer="random_normal"))
model.add(keras.layers.Dense(1, name="dense2", kernel_initializer='orthogonal', bias_initializer='random_normal'))

# compile model
model.compile(loss='mse', optimizer='adam')

# train model
model.fit(x_train_items_add_batch, y_train_items_add_batch, validation_data=(x_validation_items_add_batch, y_validation_items_add_batch), epochs=10, batch_size=32, shuffle="false", callbacks=[TqdmCallback(verbose=0)])
scores = model.evaluate(x_test_items_add_batch, y_test_items_add_batch)
print(scores)
print(model.metrics_names)
save_model(model, 'neural_net_weights.json')
