import keras
import sys
import pandas
import numpy as np
import cPickle
from sklearn.ensemble import RandomForestClassifier
from utilities import load_test_wav_features


class BaseModel(object):

    """
    Base class for all models.

    Using as base interface for all models. In BaseModel class function
    using functions, that should be implemented in child classes:

        - fit(X_train, y_train, train_mode)
        - predict_proba(one_sample)
        - predict_class(one_sample)
        - test(X_test, y_test)
        - load_model_dump(path)
        - save_model_dump(path)
        - create_new_model(model_parameters)

    """

    TRAIN_MODES         = ['multiclass', 'one-vs-all']
    CREATE_MODEL_MODES  = ['load', 'create']
    PREDICT_MODES       = ['probabilities', 'class-id']

    def __init__(self, path_to_dump):
        if not path_to_dump:
            raise Exception('BaseModel::__init__(...). Incorrect "path_to_dump" parameter value.')

        self.path_to_dump = path_to_dump
        self.model = None

    def _check_input_data(self, X, y):
        # check input data correctness
        if len(y) == 0:
            raise Exception('BaseModel::_check_input_data(...). Empty "y_train" values.')
        if len(X) == 0:
            raise Exception('BaseModel::_check_input_data(...). Empty "X_train" values.')
        if not all(len(x) == len(X[0]) for x in X):
            raise Exception('BaseModel::_check_input_data(...). Train samples lengths in X_train are not equal.')

    def load_or_create(self, create_mode):
        if create_mode.lower() not in BaseModel.CREATE_MODEL_MODES:
            raise Exception(
                'BaseModel::_load_or_create(...). Invalid "create_mode" parameter value (got {0} expected one of {1})'.format(
                    create_mode, BaseModel.CREATE_MODEL_MODES
            ))

        # create or load model
        if create_mode == 'load':
            self.load_model_dump()
        else:
            self.create_new_model()

    def train_and_save(self, X_train, y_train, train_mode='multiclass'):
        # check parameters
        self._check_input_data(X_train, y_train)
        
        if train_mode.lower() not in BaseModel.TRAIN_MODES:
            raise Exception(
                'BaseModel::train_and_save(...). Invalid "train_mode" parameter value (got {0} expected one of {1})'.format(
                    train_mode, BaseModel.TRAIN_MODES
            ))

        # can create and train model if input data is correct
        self.load_or_create(create_mode='create')
        self.fit(X_train, y_train)

        # save dump
        self.save_model_dump()

    def load_and_test(self, X_test, y_test):
        # check parameters
        self._check_input_data(X_test, y_test)

        # can create and train model if input data is correct
        self.load_or_create(create_mode='load')
        return self.test(X_test, y_test)

    def predict_one_wav(self, path_to_wav_features, path_to_save_results, predict_mode='probabilities'):
        if predict_mode.lower() not in BaseModel.PREDICT_MODES:
            raise Exception(
                'BaseModel::predict_one_wav(...). Invalid "predict_mode" parameter value (got {0} expected one of {1})'.format(
                    predict_mode, BaseModel.PREDICT_MODES
            ))

        # load saved previously model
        self.load_or_create(create_mode='load')

        # load wav file features
        features = load_test_wav_features(path_to_wav_features)

        # predict classes
        if predict_mode == 'probabilities':
            prediction = self.predict_proba(features)
        else:
            prediction = self.predict_class(features)

        # save result in output file
        with open(path_to_save_results, 'w') as outf:
            outf.write(' '.join(map(str, prediction)))


class NeuralNetModel(BaseModel):
    def __init__(
        self
        , path_to_dump
        , input_dim=None
        , number_of_classes=None
        , loss='sparse_categorical_crossentropy'
        , optimizer='adam'
        , metrics=['accuracy']
    ):
        super(NeuralNetModel, self).__init__(path_to_dump)

        self.input_dim = input_dim
        self.number_of_classes = number_of_classes
        self.loss = loss
        self.optimizer = optimizer
        self.metrics = metrics

    def create_new_model(self):
        model = keras.models.Sequential()
        model.add(keras.layers.Dense(self.input_dim, input_dim=self.input_dim, activation='relu'))
        model.add(keras.layers.Dense(self.input_dim, activation='relu'))
        model.add(keras.layers.Dense(self.number_of_classes, activation='sigmoid'))
        model.compile(loss=self.loss, optimizer=self.optimizer, metrics=self.metrics)
        self.model = model

    def load_model_dump(self):
        self.model = keras.models.load_model(self.path_to_dump)

    def save_model_dump(self):
        self.model.save(self.path_to_dump)

    def fit(self, X_train, y_train):
        self.model.fit(X_train, y_train)

    def predict_proba(self, test_sample):
        self.model.predict_proba(test_sample)[0]

    def predict_class(self, test_sample):
        return self.model.predict_classes(test_sample)

    def test(self, X_test, y_test):
        return zip(self.model.metrics_names, self.model.evaluate(X_test, y_test, verbose=False))


class RandomForestModel(BaseModel):
    def __init__(self, path_to_dump, n_estimators=300):
        super(NeuralNetModel, self).__init__(path_to_dump)

        self.n_estimators = n_estimators

    def create_new_model(self):
        self.model = RandomForestClassifier(n_estimators=self.n_estimators)
    
    def load_model_dump(self):
        with open(self.path_to_dump, 'rb') as inf:
            self.model = cPickle.load(inf)

    def save_model_dump(self):
        with open(self.path_to_dump, 'wb') as outf:
            cPickle.dump(self.model, outf)

    def fit(self, X_train, y_train):
        self.model.fit(X_train, y_train)

    def predict_proba(self, test_sample):
        return self.model.predict_proba([features])[0]

    def predict_class(self, test_sample):
        return self.model.predict([features])[0]

    def test(self, X_test, y_test):
        return self.model.score(X_test, y_test)


POSSIBLE_MODELS_NAMES = [
    'NeuralNetModel'
    , 'RandomForestModel'
]
