import keras
import sys
import pandas
import numpy


def create_nn(input_dim_=13):
    model = keras.models.Sequential()
    model.add(keras.layers.Dense(64, input_dim=input_dim_, activation='relu'))
    model.add(keras.layers.core.Dropout(0.2))
    model.add(keras.layers.Dense(32, activation='relu'))
    model.add(keras.layers.Dense(1, activation='sigmoid'))
    model.compile(loss='mean_squared_error', optimizer='sgd', metrics=['binary_accuracy'])
    return model


def train_nn(path_to_train, path_to_test, path_to_save_nn_dump, input_dim):
    df, X = pandas.read_csv(path_to_train), []
    for record in df['Features'].values:
        X.extend([float(a) for a in record.split(' ')[:-1]])
    
    X = numpy.array(X).reshape(len(df), (len(X) / len(df)))
    Y = df['Class'].values

    nn = create_nn(input_dim_=X.shape[1])
    nn.fit(X, Y)
    nn.save(path_to_save_nn_dump)
    del nn


def predict(test_record_filepath, load_model_from, path_to_store_result):
    nn = keras.models.load_model(load_model_from)
    with open(test_record_filepath, 'r') as inf:
        features = map(lambda x: float(x), inf.read().split(' ')[:-1])
    
    result = nn.predict(numpy.array([features]))
    with open(path_to_store_result, 'w') as outf:
        outf.write(str(int(round(result[0][0]))))


if __name__ == '__main__':
    if sys.argv[1] == 'fit':
        train_nn(*sys.argv[2:])
    elif sys.argv[1] == 'predict':
        predict(*sys.argv[2:])
