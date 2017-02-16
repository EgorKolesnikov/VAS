import keras
import sys
import pandas
import numpy


class BaseModel:
    def __init__(self):
        pass

    def train_and_test(self, X_train, y_train, X_test, y_test):
        pass


class NeuralNetModel(BaseModel):
    def __init__(self):
        pass

    def train_and_test(self, X_train, y_train, X_test, y_test):
        pass


def create_nn(input_dim_):
    model = keras.models.Sequential()
    model.add(keras.layers.Dense(64, input_dim=input_dim_, activation='relu'))
    model.add(keras.layers.core.Dropout(0.2))
    model.add(keras.layers.Dense(32, activation='relu'))
    model.add(keras.layers.Dense(1, activation='sigmoid'))
    model.compile(loss='mean_squared_error', optimizer='sgd', metrics=['binary_accuracy'])
    return model


def train_nn(path_to_train, path_to_test, path_to_save_nn_dump, path_to_save_training_score_info):
    # load training data
    df, X_train = pandas.read_csv(path_to_train), []
    for record in df['Features'].values:
        X_train.extend([float(a) for a in record.split(' ')[:-1]])
    X_train = numpy.array(X_train).reshape(len(df), (len(X_train) / len(df)))
    y_train = df['Class'].values

    # load testing data
    df, X_test = pandas.read_csv(path_to_test), []
    for record in df['Features'].values:
        X_test.extend([float(a) for a in record.split(' ')[:-1]])
    X_test = numpy.array(X_test).reshape(len(df), (len(X_test) / len(df)))
    y_test = df['Class'].values

    # create model and train
    model = create_nn(input_dim_=X_train.shape[1])
    model.fit(X_train, y_train)

    # testing (if we have test data)
    if len(y_test) > 0:
        scores = model.evaluate(X_test, y_test, verbose=0)

        # write train and test score in file
        with open(path_to_save_training_score_info, 'w') as outf:
            outf.write("Train size: {0}. Test size: {1}.\n".format(len(y_train), len(y_test)))
            outf.write("Score on test data:  {}\n".format("%s: %.2f%%" % (model.metrics_names[1], scores[0]*100)))
            outf.write("Score on train data: {}\n".format("%s: %.2f%%" % (model.metrics_names[1], scores[1]*100)))

    # saving dump
    model.save(path_to_save_nn_dump)
    del model


def predict(testing_record_features_path, path_to_saved_nn_dump, path_to_store_result):
    nn = keras.models.load_model(path_to_saved_nn_dump)
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
