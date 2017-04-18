import sys
import numpy as np
import models
from utilities import load_file_info, load_test_wav_features, FeaturesPreprocess

from collections import defaultdict, Counter


# according to source/settings.h
FEATURES_PREPROCESS = {
    0: FeaturesPreprocess.no_preprocess
    , 1: FeaturesPreprocess.normalization
    , 2: FeaturesPreprocess.pca_whitening
}

def fit(
    path_to_train_data             # path to text file with train data (strict format with header)
    , path_to_test_data            # path to test file with test data (strict format with header)
    , path_to_save_nn_dump         # path to save model dump
    , model_name                   # one of models.py::MAIN_MODELS_NAMES
    , preprocess_type              # features preprocess routine id (one of FEATURES_PREPROCESS)
    , main_voice_in_preprocess     # id of main voice in preprocess routine
):
    # prepare_parameters
    features_preprocess_function = FEATURES_PREPROCESS.get(int(preprocess_type), None)
    preprocess_main_class = int(main_voice_in_preprocess)

    if preprocess_main_class < 0:
        preprocess_main_class = None

    # log
    print 'Running train procedure with:\n - train from: {}\n - model name: {}\n - preprocess: {}\n - main_voice: {}\n'.format(
        path_to_train_data, model_name, features_preprocess_function.__name__, preprocess_main_class
    )

    # load data
    X_train, y_train = load_file_info(path_to_train_data)
    X_test, y_test = load_file_info(path_to_test_data)

    # preprocess if we need to
    X_train, preprocess_secondary_data = features_preprocess_function(
        X_train
        , y=y_train
        , main_class=preprocess_main_class
        , mode='train'
        , preprocess_help_data=None
    )

    # create specified model
    if model_name == 'NN':
        model = models.NeuralNetModel(
            path_to_save_nn_dump
            , input_dim=len(X_train[0])
            , number_of_classes=len(set(list(y_train) + list(y_test)))
        )
    elif model_name == 'RF':
        model = models.RandomForestModel(
            path_to_save_nn_dump
            , n_estimators=300
        )
    else:
        raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))

    # save secondary data from preprocess routine in model object
    # we will need that data in testing procedure (predict(...))
    model.model_secondary_data['preprocess_routine_type'] = preprocess_type
    model.model_secondary_data['preprocess_main_voice_class'] = preprocess_main_class
    model.model_secondary_data['preprocess_routine_secondary_data'] = tuple(item.tolist() for item in preprocess_secondary_data)

    # log
    print 'Saving train preprocess data: {}'.format(preprocess_secondary_data)

    # fit and save dump (secondary data will be saved also)
    model.train_and_save(X_train, y_train)

    # return test score if we have test data
    if len(y_test) != 0:
        X_test, preprocess_secondary_data = features_preprocess_function(
            X_test
            , y=y_test
            , main_class=preprocess_main_class
            , mode='train'
            , preprocess_help_data=None
        )

        test_result = model.test(X_test, y_test)
        print 'Test result: {0}'.format(test_result)


def predict(
    testing_record_features_path
    , path_to_saved_model_dump
    , path_to_store_result
    , model_name='NN'
):
    # create specified model
    if model_name == 'NN':
        model = models.NeuralNetModel(path_to_saved_model_dump)
    elif model_name == 'RF':
        model = models.RandomForestModel(path_to_saved_model_dump)
    else:
        raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))
    model.load_or_create(create_mode='load')

    # log
    print 'Running test procedure with:\n - model dump from: {}\n'.format(
        path_to_saved_model_dump
    )

    # load data for preprocess from model secondary data
    preprocess_type = model.model_secondary_data['preprocess_routine_type']
    preprocess_main_class = model.model_secondary_data['preprocess_main_voice_class']
    preprocess_secondary_data = model.model_secondary_data['preprocess_routine_secondary_data']

    preprocess_function = FEATURES_PREPROCESS.get(int(preprocess_type), None)

    # log
    print 'Running test procedure with:\n - model dump from: {}\n - preprocess function: {}\n - preprocess data: {}\n'.format(
        path_to_saved_model_dump, preprocess_function.__name__, preprocess_secondary_data
    )

    # load wav file features
    test_frames_features = load_test_wav_features(testing_record_features_path)

    # run features preprocess procedure
    test_frames_features = preprocess_function(
        test_frames_features
        , y=None
        , main_class=preprocess_main_class
        , mode='test'
        , preprocess_help_data=preprocess_secondary_data
    )

    # run classification
    count_decisions = Counter()

    # classify each frame
    for frame_features in test_frames_features:
        classification_results = model.predict_one_wav(np.array([frame_features]), predict_mode='probabilities')
        result_class = np.argmax(classification_results) + 1
        count_decisions[result_class] += 1
        # print result_class, classification_results

    # combine each frame classification results
    result_class = count_decisions.most_common(1)[0][0]

    assert(result_class is not None)
    with open(path_to_store_result, 'w') as outf:
        outf.write(str(result_class))


if __name__ == '__main__':
    if sys.argv[1] == 'fit':
        fit(*sys.argv[2:])
    elif sys.argv[1] == 'predict':
        predict(*sys.argv[2:])
